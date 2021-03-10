#pragma once

#include "erm/rendering/shaders/IShaderProgram.h"

namespace erm {

	class ShaderProgram : public IShaderProgram
	{
	public:
		ShaderProgram(Device& device, const char* path);
	};

} // namespace erm