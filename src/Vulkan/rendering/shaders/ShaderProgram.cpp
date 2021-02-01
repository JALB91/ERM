#include "erm/rendering/shaders/ShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/VertexData.h"

#include "erm/utils/Utils.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

namespace {

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

	vk::ShaderModule CreateShaderModule(vk::Device device, const std::vector<char>& code)
	{
		vk::ShaderModuleCreateInfo createInfo = {};
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		return device.createShaderModule(createInfo);
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

	void GetBindingData(spirv_cross::Compiler& vertCompiler, spirv_cross::Compiler& fragCompiler, std::vector<erm::UboData>& ubosData, std::vector<erm::SamplerData>& samplerData)
	{
		spirv_cross::ShaderResources resources = vertCompiler.get_shader_resources();

		for (const spirv_cross::Resource& res : resources.uniform_buffers)
			ubosData.emplace_back(GetUboData(vertCompiler, res));

		for (const spirv_cross::Resource& res : resources.sampled_images)
			samplerData.emplace_back(GetSamplerData(vertCompiler, res));

		resources = fragCompiler.get_shader_resources();

		for (const spirv_cross::Resource& res : resources.uniform_buffers)
			ubosData.emplace_back(GetUboData(fragCompiler, res));

		for (const spirv_cross::Resource& res : resources.sampled_images)
			samplerData.emplace_back(GetSamplerData(fragCompiler, res));
	}

} // namespace

namespace erm {

	unsigned int ShaderProgram::sShaderId = 0;

	ShaderProgram::ShaderProgram(Device& device, const std::string& shaderPath)
		: mDevice(device)
		, mPath(shaderPath)
		, mVertexSource(Utils::ReadFromFile((mPath + ".vert").c_str()))
		, mFragmentSource(Utils::ReadFromFile((mPath + ".frag").c_str()))
		, mVertex(ReadShaderCompiled((mPath + ".vert.cmp").c_str()))
		, mFragment(ReadShaderCompiled((mPath + ".frag.cmp").c_str()))
		, mVertCompiler(std::make_unique<spirv_cross::Compiler>(LoadSpirvFile((mPath + ".vert.cmp").c_str())))
		, mFragCompiler(std::make_unique<spirv_cross::Compiler>(LoadSpirvFile((mPath + ".frag.cmp").c_str())))
		, mNeedsReload(true)
	{
		::GetBindingData(*mVertCompiler, *mFragCompiler, mUbosData, mSamplerData);
	}

	ShaderProgram::ShaderProgram(Device& device, const char* vertShader, const char* fragShader)
		: mDevice(device)
		, mPath("TEMP_" + std::to_string(sShaderId++))
	{
		SetShaderSources(vertShader, fragShader);
	}

	void ShaderProgram::SetShaderSources(const std::string& vertex, const std::string& fragment)
	{
		// VERTEX SHADER
		mVertexSource = vertex;
		mVertex.clear();

		Utils::WriteToFile((mPath + "_tmp.vert").c_str(), mVertexSource);

		{
			std::string compilationCommand = ERM_SHADER_COMPILER;
			compilationCommand += " " + mPath + "_tmp.vert -o " + mPath + ".vert.cmp";
			system(compilationCommand.c_str());
		}

		mVertex = ReadShaderCompiled((mPath + ".vert.cmp").c_str());
		mVertCompiler = std::make_unique<spirv_cross::Compiler>(LoadSpirvFile((mPath + ".vert.cmp").c_str()));

		// FRAGMENT SHADER
		mFragmentSource = fragment;
		mFragment.clear();

		Utils::WriteToFile((mPath + "_tmp.frag").c_str(), mFragmentSource);

		{
			std::string compilationCommand = ERM_SHADER_COMPILER;
			compilationCommand += " " + mPath + "_tmp.frag -o " + mPath + ".frag.cmp";
			system(compilationCommand.c_str());
		}

		mFragment = ReadShaderCompiled((mPath + ".frag.cmp").c_str());
		mFragCompiler = std::make_unique<spirv_cross::Compiler>(LoadSpirvFile((mPath + ".frag.cmp").c_str()));

		mUbosData.clear();
		mSamplerData.clear();

		::GetBindingData(*mVertCompiler, *mFragCompiler, mUbosData, mSamplerData);

		mNeedsReload = true;
	}

	vk::ShaderModule ShaderProgram::CreateVertexShaderModule() const
	{
		return CreateShaderModule(mDevice.GetVkDevice(), mVertex);
	}

	vk::ShaderModule ShaderProgram::CreateFragmentShaderModule() const
	{
		return CreateShaderModule(mDevice.GetVkDevice(), mFragment);
	}

	vk::VertexInputBindingDescription ShaderProgram::GetVertexBindingDescription()
	{
		spirv_cross::ShaderResources resources = mVertCompiler->get_shader_resources();

		vk::VertexInputBindingDescription bindingDescription = {};

		if (!resources.stage_inputs.empty())
		{
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(VertexData);
			bindingDescription.inputRate = vk::VertexInputRate::eVertex;
		}

		return bindingDescription;
	}

	std::vector<vk::VertexInputAttributeDescription> ShaderProgram::GetVertexAttributeDescriptions()
	{
		spirv_cross::ShaderResources resources = mVertCompiler->get_shader_resources();

		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

		for (const spirv_cross::Resource& res : resources.stage_inputs)
		{
			vk::VertexInputAttributeDescription description;

			description.binding = mVertCompiler->get_decoration(res.id, spv::Decoration::DecorationBinding);
			description.location = mVertCompiler->get_decoration(res.id, spv::Decoration::DecorationLocation);

			if (res.name.compare("inPosition") == 0)
			{
				description.format = vk::Format::eR32G32B32Sfloat;
				description.offset = offsetof(VertexData, mPositionVertex);
			}
			else if (res.name.compare("inNormal") == 0 || res.name.compare("inColor") == 0)
			{
				description.format = vk::Format::eR32G32B32Sfloat;
				description.offset = offsetof(VertexData, mNormalVertex);
			}
			else if (res.name.compare("inTexCoord") == 0)
			{
				description.format = vk::Format::eR32G32Sfloat;
				description.offset = offsetof(VertexData, mUVVertex);
			}
			else if (res.name.compare("inBoneWeights") == 0)
			{
				description.format = vk::Format::eR32G32B32A32Sfloat;
				description.offset = offsetof(VertexData, mBoneWeights);
			}
			else if (res.name.compare("inBoneIds") == 0)
			{
				description.format = vk::Format::eR32G32B32A32Uint;
				description.offset = offsetof(VertexData, mBoneIds);
			}
			else if (res.name.compare("inBoneNum") == 0)
			{
				description.format = vk::Format::eR32Sint;
				description.offset = offsetof(VertexData, mBoneNum);
			}

			attributeDescriptions.emplace_back(std::move(description));
		}

		return attributeDescriptions;
	}

	std::vector<vk::DescriptorSetLayoutBinding> ShaderProgram::GetDescriptorSetLayoutBindings()
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings;

		GatherDescriptorSetLayoutBindings(bindings, *mVertCompiler, vk::ShaderStageFlagBits::eVertex);
		GatherDescriptorSetLayoutBindings(bindings, *mFragCompiler, vk::ShaderStageFlagBits::eFragment);

		return bindings;
	}

} // namespace erm
