#include "erm/rendering/shaders/ShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/VertexData.h"

#include "erm/utils/Utils.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

namespace {

	std::vector<char> ReadShader(const char* path)
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
		std::vector<char> file = ReadShader(path);

		std::vector<uint32_t> buffer(file.size() / sizeof(uint32_t));
		memcpy(buffer.data(), file.data(), file.size());

		return buffer;
	}

	void GatherDescriptorSetLayoutBindings(std::vector<vk::DescriptorSetLayoutBinding>& bindings, spirv_cross::CompilerGLSL& compiler, vk::ShaderStageFlagBits flags)
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

	erm::UboData GetUboData(spirv_cross::CompilerGLSL& compiler, const spirv_cross::Resource& resource)
	{
		const spirv_cross::SmallVector<spirv_cross::BufferRange>& ranges = compiler.get_active_buffer_ranges(resource.id);

		if (ranges.size() == 1)
		{
			const spirv_cross::BufferRange& range = ranges[0];

			std::string name = compiler.get_member_name(resource.base_type_id, range.index);

			std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
				return std::tolower(c);
			});

			if (name.find("mvp") != std::string::npos && range.range == sizeof(erm::UboBasic))
				return {erm::UboBasic::ID, sizeof(erm::UboBasic), compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset), compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding)};
		}

		ASSERT(false);

		return {erm::UboBasic::ID, sizeof(erm::UboBasic), 0, 0};
	}

	std::vector<erm::UboData> GetUbosData(spirv_cross::CompilerGLSL& compiler)
	{
		std::vector<erm::UboData> data;

		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (const spirv_cross::Resource& res : resources.uniform_buffers)
			data.emplace_back(GetUboData(compiler, res));

		return data;
	}

} // namespace

namespace erm {

	ShaderProgram::ShaderProgram(Device& device, const std::string& shaderPath)
		: mDevice(device)
		, mPath(shaderPath)
		, mVertex(ReadShader((mPath + ".vert").c_str()))
		, mFragment(ReadShader((mPath + ".frag").c_str()))
		, mVertCompiler(LoadSpirvFile((mPath + ".vert").c_str()))
		, mFragCompiler(LoadSpirvFile((mPath + ".frag").c_str()))
		, mVertUbosData(::GetUbosData(mVertCompiler))
		, mFragUbosData(::GetUbosData(mFragCompiler))
	{}

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
		spirv_cross::ShaderResources resources = mVertCompiler.get_shader_resources();

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
		spirv_cross::ShaderResources resources = mVertCompiler.get_shader_resources();

		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

		for (const spirv_cross::Resource& res : resources.stage_inputs)
		{
			vk::VertexInputAttributeDescription description;

			description.binding = mVertCompiler.get_decoration(res.id, spv::Decoration::DecorationBinding);
			description.location = mVertCompiler.get_decoration(res.id, spv::Decoration::DecorationLocation);

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

			attributeDescriptions.emplace_back(std::move(description));
		}

		return attributeDescriptions;
	}

	std::vector<vk::DescriptorSetLayoutBinding> ShaderProgram::GetDescriptorSetLayoutBindings()
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings;

		GatherDescriptorSetLayoutBindings(bindings, mVertCompiler, vk::ShaderStageFlagBits::eVertex);
		GatherDescriptorSetLayoutBindings(bindings, mFragCompiler, vk::ShaderStageFlagBits::eFragment);

		return bindings;
	}

} // namespace erm
