#include "erm/rendering/shaders/ShaderProgram.h"

#include "erm/rendering/Device.h"

#include <vulkan/vulkan.hpp>

#include <fstream>
#include <iostream>

namespace {

	std::vector<char> ReadShader(const std::string& path)
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

} // namespace

namespace erm {

	ShaderProgram::ShaderProgram(Device& device, const std::string& shaderPath)
		: mDevice(device)
		, mPath(shaderPath)
		, mVertex(ReadShader(mPath + ".vert"))
		, mFragment(ReadShader(mPath + ".frag"))
	{}

	vk::ShaderModule ShaderProgram::CreateVertexShaderModule() const
	{
		return CreateShaderModule(mDevice.GetVkDevice(), mVertex);
	}

	vk::ShaderModule ShaderProgram::CreateFragmentShaderModule() const
	{
		return CreateShaderModule(mDevice.GetVkDevice(), mFragment);
	}

} // namespace erm
