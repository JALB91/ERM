#pragma once

#include "erm/rendering/data_structs/BindingConfigs.h"

namespace erm {
class RTShaderProgram;
}

namespace erm {

struct RTRenderConfigs : public BindingConfigs
{
	RTRenderConfigs()
		: mShaderProgram(nullptr)
	{}

	RTShaderProgram* mShaderProgram;
};

} // namespace erm
