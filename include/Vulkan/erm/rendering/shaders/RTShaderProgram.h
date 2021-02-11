#pragma once

#ifdef ERM_RAY_TRACING_ENABLED

#	include "erm/rendering/shaders/IShaderProgram.h"

namespace erm {

	class RTShaderProgram : public IShaderProgram
	{
	public:
		RTShaderProgram(Device& device, const char* shaderPath);
	};

} // namespace erm

#endif