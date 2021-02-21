#include "erm/rendering/shaders/IShaderProgram.h"

#include "erm/rendering/Device.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

namespace {

	const char* GetExtensionForShaderType(erm::ShaderType shaderType)
	{
		switch (shaderType)
		{
			case erm::ShaderType::VERTEX:
				return ".vert";
			case erm::ShaderType::FRAGMENT:
				return ".frag";
			case erm::ShaderType::RT_ANY_HIT:
				return ".rahit";
			case erm::ShaderType::RT_CALLABLE:
				return ".rcall";
			case erm::ShaderType::RT_CLOSEST_HIT:
				return ".rchit";
			case erm::ShaderType::RT_INTERSECTION:
				return ".rint";
			case erm::ShaderType::RT_MISS:
				return ".rmiss";
			case erm::ShaderType::RT_RAY_GEN:
				return ".rgen";
			default:
				ASSERT(false);
				return "";
		}
	}

	std::vector<char> ReadShaderCompiled(const char* path)
	{
		std::ifstream stream(path, std::ios::ate | std::ios::binary);

		if (!stream.is_open())
		{
			throw std::runtime_error("Failed to open shader file");
		}

		size_t fileSize = static_cast<size_t>(stream.tellg());
		std::vector<char> buffer(fileSize);

		stream.seekg(0);
		stream.read(buffer.data(), fileSize);

		stream.close();

		return buffer;
	}

	std::vector<uint32_t> LoadSpirvFile(const char* path)
	{
		std::vector<char> file = ReadShaderCompiled(path);

		std::vector<uint32_t> buffer(file.size() / sizeof(uint32_t));
		memcpy(buffer.data(), file.data(), file.size());

		return buffer;
	}

	erm::UboData GetUboData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
	{
		const auto makeUboData = [&compiler, &resource](erm::UboId id, size_t size) -> erm::UboData {
			return {
				id,
				size,
				compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset),
				compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
				compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet)};
		};

		if (resource.name.compare("UniformBufferObject") == 0)
			return makeUboData(erm::UboBasic::ID, sizeof(erm::UboBasic));
		else if (resource.name.compare("ModelViewProj") == 0)
			return makeUboData(erm::UboModelViewProj::ID, sizeof(erm::UboModelViewProj));
		else if (resource.name.compare("Material") == 0)
			return makeUboData(erm::UboMaterial::ID, sizeof(erm::UboMaterial));
		else if (resource.name.compare("Light") == 0)
			return makeUboData(erm::UboLight::ID, sizeof(erm::UboLight));
		else if (resource.name.compare("View") == 0)
			return makeUboData(erm::UboView::ID, sizeof(erm::UboView));
		else if (resource.name.compare("Skeleton") == 0)
			return makeUboData(erm::UboSkeleton::ID, sizeof(erm::UboSkeleton));
		else if (resource.name.compare("PBMaterial") == 0)
			return makeUboData(erm::UboPBMaterial::ID, sizeof(erm::UboPBMaterial));
		else if (resource.name.compare("PBLight") == 0)
			return makeUboData(erm::UboPBLight::ID, sizeof(erm::UboPBLight));
		else if (resource.name.compare("BonesDebug") == 0)
			return makeUboData(erm::UboBonesDebug::ID, sizeof(erm::UboBonesDebug));

		ASSERT(false);

		return {erm::UboBasic::ID, sizeof(erm::UboBasic), 0, 0, 0};
	}

	erm::SamplerData GetSamplerData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
	{
		const auto makeSamplerData = [&compiler, &resource](erm::TextureType type) -> erm::SamplerData {
			return {
				compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
				compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet),
				type};
		};

		if (resource.name.compare("diffuseSampler") == 0)
			return makeSamplerData(erm::TextureType::DIFFUSE);
		else if (resource.name.compare("normalSampler") == 0)
			return makeSamplerData(erm::TextureType::NORMAL);
		else if (resource.name.compare("specularSampler") == 0)
			return makeSamplerData(erm::TextureType::SPECULAR);

		ASSERT(false);

		return {0, 0, erm::TextureType::DIFFUSE};
	}

	erm::StorageImageData GetStorageImageData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
	{
		const auto makeStorageImageData = [&compiler, &resource]() -> erm::StorageImageData {
			return {
				compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
				compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet)};
		};

		if (resource.name.compare("imageStore") == 0)
			return makeStorageImageData();

		ASSERT(false);

		return {0, 0};
	}

	erm::StorageBufferData GetStorageBufferData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
	{
		const auto makeStorageBufferData = [&compiler, &resource](erm::StorageBufferType type) -> erm::StorageBufferData {
			return {
				type,
				VK_WHOLE_SIZE,
				compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset),
				compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
				compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet)};
		};

		if (resource.name.compare("Vertices") == 0)
			return makeStorageBufferData(erm::StorageBufferType::VERTICES);
		else if (resource.name.compare("Indices") == 0)
			return makeStorageBufferData(erm::StorageBufferType::INDICES);

		ASSERT(false);

		return {erm::StorageBufferType::VERTICES, VK_WHOLE_SIZE, 0, 0, 0};
	}

	void GatherResourceBindings(
		erm::ShaderBindingData& data,
		const spirv_cross::Compiler& compiler,
		const spirv_cross::Resource& res,
		vk::ShaderStageFlagBits flags,
		vk::DescriptorType type)
	{
		const uint32_t binding = compiler.get_decoration(res.id, spv::Decoration::DecorationBinding);

		vk::DescriptorSetLayoutBinding& layoutBinding = data.mLayoutBindings.emplace_back();
		layoutBinding.binding = binding;
		layoutBinding.descriptorCount = 1;
		layoutBinding.descriptorType = type;
		layoutBinding.stageFlags = flags;

		switch (type)
		{
			case vk::DescriptorType::eUniformBuffer:
				data.mUbosData.emplace_back(GetUboData(compiler, res));
				break;
			case vk::DescriptorType::eCombinedImageSampler:
				data.mSamplersData.emplace_back(GetSamplerData(compiler, res));
				break;
			case vk::DescriptorType::eStorageImage:
				data.mStorageImagesData.emplace_back(GetStorageImageData(compiler, res));
				break;
			case vk::DescriptorType::eStorageBuffer:
				data.mStorageBuffersData.emplace_back(GetStorageBufferData(compiler, res));
				break;
			default:
				ASSERT(false);
		}
	}

	void GatherShaderBindings(erm::ShaderBindingsMap& bindings, const spirv_cross::Compiler& compiler, vk::ShaderStageFlagBits flags)
	{
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (const spirv_cross::Resource& ubo : resources.uniform_buffers)
		{
			const uint32_t targetSet = compiler.get_decoration(ubo.id, spv::Decoration::DecorationDescriptorSet);
			GatherResourceBindings(bindings[targetSet], compiler, ubo, flags, vk::DescriptorType::eUniformBuffer);
		}

		for (const spirv_cross::Resource& sampledImage : resources.sampled_images)
		{
			const uint32_t targetSet = compiler.get_decoration(sampledImage.id, spv::Decoration::DecorationDescriptorSet);
			GatherResourceBindings(bindings[targetSet], compiler, sampledImage, flags, vk::DescriptorType::eCombinedImageSampler);
		}

		for (const spirv_cross::Resource& storageImage : resources.storage_images)
		{
			const uint32_t targetSet = compiler.get_decoration(storageImage.id, spv::Decoration::DecorationDescriptorSet);
			GatherResourceBindings(bindings[targetSet], compiler, storageImage, flags, vk::DescriptorType::eStorageImage);
		}

		for (const spirv_cross::Resource& storageBuffer : resources.storage_buffers)
		{
			const uint32_t targetSet = compiler.get_decoration(storageBuffer.id, spv::Decoration::DecorationDescriptorSet);
			GatherResourceBindings(bindings[targetSet], compiler, storageBuffer, flags, vk::DescriptorType::eStorageBuffer);
		}
	}

} // namespace

namespace erm {

	IShaderProgram::IShaderProgram(Device& device, const char* shaderPath)
		: IAsset(shaderPath, "")
		, mDevice(device)
		, mNeedsReload(true)
	{}

	vk::ShaderModule IShaderProgram::CreateShaderModule(ShaderType shaderType) const
	{
		const auto it = mShadersData.find(shaderType);
		if (it == mShadersData.end())
			return nullptr;

		const ShaderData& data = it->second;

		vk::ShaderModuleCreateInfo createInfo = {};
		createInfo.codeSize = data.mShaderByteCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(data.mShaderByteCode.data());

		return mDevice->createShaderModule(createInfo);
	}

	void IShaderProgram::UpdateBindingData()
	{
		mShaderBindingsMap.clear();

		for (const auto& [shaderType, data] : mShadersData)
		{
			ASSERT(data.mShaderCompiler);
			GatherShaderBindings(mShaderBindingsMap, *data.mShaderCompiler, VkUtils::ToVulkanValue<vk::ShaderStageFlagBits>(shaderType));
		}
	}

	void IShaderProgram::UpdateShaderData(ShaderType shaderType)
	{
		ShaderData& data = mShadersData[shaderType];

		const std::string shaderPath = mPath + GetExtensionForShaderType(shaderType);
		const std::string compiledShaderPath = shaderPath + ".cmp";

		data.mShaderSource = Utils::ReadFromFile(shaderPath.c_str());
		data.mShaderByteCode = ReadShaderCompiled(compiledShaderPath.c_str());
		data.mShaderCompiler = std::make_unique<spirv_cross::Compiler>(LoadSpirvFile(compiledShaderPath.c_str()));
	}

	void IShaderProgram::CompileShaderSource(ShaderType shaderType) const
	{
		const std::string shaderPath = mPath + GetExtensionForShaderType(shaderType);
		const std::string compiledShaderPath = shaderPath + ".cmp";
		std::string compilationCommand = ERM_SHADER_COMPILER;
		compilationCommand += " " + shaderPath + " -o " + compiledShaderPath;
		system(compilationCommand.c_str());
	}

	const ShaderData& IShaderProgram::GetShaderData(ShaderType shaderType) const
	{
		const auto it = mShadersData.find(shaderType);
		ASSERT(it != mShadersData.end());
		return it->second;
	}

	ShaderData& IShaderProgram::GetShaderData(ShaderType shaderType)
	{
		const auto it = mShadersData.find(shaderType);
		ASSERT(it != mShadersData.end());
		return it->second;
	}

	const ShaderBindingData& IShaderProgram::GetShaderBindingsData(SetIdx setIdx) const
	{
		const auto it = mShaderBindingsMap.find(setIdx);
		ASSERT(it != mShaderBindingsMap.end());
		return it->second;
	}

} // namespace erm