#pragma once

#include "erm/rendering/shaders/IShaderProgram.h"

namespace erm {

	class RTShaderProgram : public IShaderProgram
	{
	public:
		RTShaderProgram(Device& device, const char* shaderPath);
	};

} // namespace erm
