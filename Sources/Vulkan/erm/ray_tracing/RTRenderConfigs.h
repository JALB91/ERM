#pragma once

#include "erm/rendering/data_structs/BindingConfigs.h"

namespace erm {
class IShaderProgram;
}

namespace erm {

struct RTRenderConfigs : public BindingConfigs
{
	RTRenderConfigs()
		: mShaderProgram(nullptr)
	{}

	IShaderProgram* mShaderProgram;
};

} // namespace erm
