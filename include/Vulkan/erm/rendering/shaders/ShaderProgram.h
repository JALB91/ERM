#pragma once

#include "erm/rendering/data_structs/UniformBufferData.h"

#include <spirv_glsl.hpp>

#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

namespace erm {
	class Device;
}

namespace erm {

	class ShaderProgram
	{
	public:
		ShaderProgram(Device& device, const std::string& shaderPath);

		ShaderProgram(ShaderProgram&&) = delete;
		ShaderProgram(const ShaderProgram&) = delete;

		ShaderProgram& operator=(ShaderProgram&&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		vk::ShaderModule CreateVertexShaderModule() const;
		vk::ShaderModule CreateFragmentShaderModule() const;

		inline const std::string& GetPath() const { return mPath; }

		vk::VertexInputBindingDescription GetVertexBindingDescription();
		std::vector<vk::VertexInputAttributeDescription> GetVertexAttributeDescriptions();
		std::vector<vk::DescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings();

		inline const spirv_cross::CompilerGLSL& GetVertCompiler() const { return mVertCompiler; }
		inline const spirv_cross::CompilerGLSL& GetFragCompiler() const { return mFragCompiler; }

		inline const std::vector<UboData>& GetVertUbosData() const { return mVertUbosData; }
		inline const std::vector<UboData>& GetFragUbosData() const { return mFragUbosData; }

	private:
		Device& mDevice;
		std::string mPath;
		std::vector<char> mVertex;
		std::vector<char> mFragment;
		spirv_cross::CompilerGLSL mVertCompiler;
		spirv_cross::CompilerGLSL mFragCompiler;
		std::vector<UboData> mVertUbosData;
		std::vector<UboData> mFragUbosData;
	};

} // namespace erm
