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
				return ".rtah";
			case erm::ShaderType::RT_CALLABLE:
				return ".rtc";
			case erm::ShaderType::RT_CLOSEST_HIT:
				return ".rtch";
			case erm::ShaderType::RT_INTERSECTION:
				return ".rti";
			case erm::ShaderType::RT_MISS:
				return ".rtm";
			case erm::ShaderType::RT_RAY_GEN:
				return ".rtrg";
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

	void GatherDescriptorSetLayoutBindings(std::vector<vk::DescriptorSetLayoutBinding>& bindings, const spirv_cross::Compiler& compiler, vk::ShaderStageFlagBits flags)
	{
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (const spirv_cross::Resource& ubo : resources.uniform_buffers)
		{
			vk::DescriptorSetLayoutBinding binding;
			binding.binding = compiler.get_decoration(ubo.id, spv::Decoration::DecorationBinding);
			binding.descriptorCount = 1;
			binding.descriptorType = vk::DescriptorType::eUniformBuffer;
			binding.stageFlags = flags;
			bindings.emplace_back(std::move(binding));
		}

		for (const spirv_cross::Resource& sampledImage : resources.sampled_images)
		{
			vk::DescriptorSetLayoutBinding binding;
			binding.binding = compiler.get_decoration(sampledImage.id, spv::Decoration::DecorationBinding);
			binding.descriptorCount = 1;
			binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			binding.stageFlags = flags;
			bindings.emplace_back(std::move(binding));
		}
	}

	erm::UboData GetUboData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
	{
		if (resource.name.compare("UniformBufferObject") == 0)
			return {erm::UboBasic::ID, sizeof(erm::UboBasic), compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset), compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding)};
		else if (resource.name.compare("ModelViewProj") == 0)
			return {erm::UboModelViewProj::ID, sizeof(erm::UboModelViewProj), compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset), compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding)};
		else if (resource.name.compare("Material") == 0)
			return {erm::UboMaterial::ID, sizeof(erm::UboMaterial), compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset), compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding)};
		else if (resource.name.compare("Light") == 0)
			return {erm::UboLight::ID, sizeof(erm::UboLight), compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset), compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding)};
		else if (resource.name.compare("View") == 0)
			return {erm::UboView::ID, sizeof(erm::UboView), compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset), compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding)};
		else if (resource.name.compare("Skeleton") == 0)
			return {erm::UboSkeleton::ID, sizeof(erm::UboSkeleton), compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset), compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding)};
		else if (resource.name.compare("PBMaterial") == 0)
			return {erm::UboPBMaterial::ID, sizeof(erm::UboPBMaterial), compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset), compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding)};
		else if (resource.name.compare("PBLight") == 0)
			return {erm::UboPBLight::ID, sizeof(erm::UboPBLight), compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset), compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding)};
		else if (resource.name.compare("BonesDebug") == 0)
			return {erm::UboBonesDebug::ID, sizeof(erm::UboBonesDebug), compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset), compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding)};

		ASSERT(false);

		return {erm::UboBasic::ID, sizeof(erm::UboBasic), 0, 0};
	}

	erm::SamplerData GetSamplerData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
	{
		if (resource.name.compare("diffuseSampler") == 0)
			return {compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding), erm::TextureType::DIFFUSE};
		else if (resource.name.compare("normalSampler") == 0)
			return {compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding), erm::TextureType::NORMAL};
		else if (resource.name.compare("specularSampler") == 0)
			return {compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding), erm::TextureType::SPECULAR};

		ASSERT(false);

		return {0, erm::TextureType::DIFFUSE};
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
		mUbosData.clear();
		mSamplerData.clear();

		for (const auto& [shaderType, data] : mShadersData)
		{
			ASSERT(data.mShaderCompiler);

			spirv_cross::ShaderResources resources = data.mShaderCompiler->get_shader_resources();

			for (const spirv_cross::Resource& res : resources.uniform_buffers)
				mUbosData.emplace_back(::GetUboData(*data.mShaderCompiler, res));

			for (const spirv_cross::Resource& res : resources.sampled_images)
				mSamplerData.emplace_back(::GetSamplerData(*data.mShaderCompiler, res));
		}
	}

	std::vector<vk::DescriptorSetLayoutBinding> IShaderProgram::GetDescriptorSetLayoutBindings() const
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings;

		for (const auto& [shaderType, data] : mShadersData)
		{
			GatherDescriptorSetLayoutBindings(bindings, *data.mShaderCompiler, VkUtils::ToVulkanValue<vk::ShaderStageFlagBits>(shaderType));
		}

		return bindings;
	}

	ShaderData IShaderProgram::GatherShaderData(ShaderType shaderType)
	{
		ShaderData data;

		const std::string shaderPath = mPath + GetExtensionForShaderType(shaderType);
		const std::string compiledShaderPath = shaderPath + ".cmp";

		data.mShaderSource = Utils::ReadFromFile(shaderPath.c_str());
		data.mShaderByteCode = ReadShaderCompiled(compiledShaderPath.c_str());
		data.mShaderCompiler = std::make_unique<spirv_cross::Compiler>(LoadSpirvFile(compiledShaderPath.c_str()));

		return data;
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

} // namespace erm