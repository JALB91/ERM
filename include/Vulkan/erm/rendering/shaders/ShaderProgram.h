#pragma once

#include "erm/rendering/data_structs/UniformBufferData.h"

#include <spirv_cpp.hpp>

#include <vulkan/vulkan.hpp>

#include <memory>
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
		ShaderProgram(Device& device, const char* vertShader, const char* fragShader);

		ShaderProgram(ShaderProgram&&) = delete;
		ShaderProgram(const ShaderProgram&) = delete;

		ShaderProgram& operator=(ShaderProgram&&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		void SetShaderSources(const std::string& vertex, const std::string& fragment);

		vk::ShaderModule CreateVertexShaderModule() const;
		vk::ShaderModule CreateFragmentShaderModule() const;

		inline const std::string& GetPath() const { return mPath; }

		vk::VertexInputBindingDescription GetVertexBindingDescription();
		std::vector<vk::VertexInputAttributeDescription> GetVertexAttributeDescriptions();
		std::vector<vk::DescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings();

		inline const std::string& GetVertexSource() const { return mVertexSource; }
		inline const std::string& GetFragmentSource() const { return mFragmentSource; }

		inline const std::vector<UboData>& GetUbosData() const { return mUbosData; }
		inline const std::vector<SamplerData>& GetSamplerData() const { return mSamplerData; }

		inline bool NeedsReload() const { return mNeedsReload; }
		inline void OnReloaded() { mNeedsReload = false; }

	private:
		static unsigned int sShaderId;

		Device& mDevice;
		std::string mPath;
		std::string mVertexSource;
		std::string mFragmentSource;
		std::vector<char> mVertex;
		std::vector<char> mFragment;
		std::unique_ptr<spirv_cross::Compiler> mVertCompiler;
		std::unique_ptr<spirv_cross::Compiler> mFragCompiler;
		std::vector<UboData> mUbosData;
		std::vector<SamplerData> mSamplerData;
		bool mNeedsReload;
	};

} // namespace erm
