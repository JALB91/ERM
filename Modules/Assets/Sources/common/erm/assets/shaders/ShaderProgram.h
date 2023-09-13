#pragma once

#include "erm/assets/enums/ShaderType.h"

namespace erm {

struct ShaderProgram
{
	ShaderProgram()
		: mShaderType(ShaderType::COUNT)
	{}
	
	ShaderType mShaderType;
};

}

