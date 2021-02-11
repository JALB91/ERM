#pragma once

#include "erm/rendering/shaders/IShaderProgram.h"

namespace erm {

	class ShaderProgram : public IShaderProgram
	{
	public:
		ShaderProgram(Device& device, const char* shaderPath);

		void SetShaderSources(const std::string& vertex, const std::string& fragment);

		vk::VertexInputBindingDescription GetVertexBindingDescription();
		std::vector<vk::VertexInputAttributeDescription> GetVertexAttributeDescriptions();
	};

} // namespace erm
