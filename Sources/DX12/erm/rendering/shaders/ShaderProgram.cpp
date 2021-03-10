#include "erm/rendering/shaders/ShaderProgram.h"

namespace erm {

	ShaderProgram::ShaderProgram(Device& device, const char* path)
		: IShaderProgram(device, path)
	{}

} // namespace erm