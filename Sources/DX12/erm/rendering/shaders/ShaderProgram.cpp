#include "erm/rendering/shaders/ShaderProgram.h"

namespace erm {

	ShaderProgram::ShaderProgram(Device& device, const char* path)
		: IAsset(path, "")
		, mDevice(device)
	{}

} // namespace erm