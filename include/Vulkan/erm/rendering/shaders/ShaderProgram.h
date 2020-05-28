#pragma once

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

	private:
		Device& mDevice;
		std::string mPath;
		std::vector<char> mVertex;
		std::vector<char> mFragment;
	};

} // namespace erm
