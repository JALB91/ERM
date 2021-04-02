#ifndef UBONESDEBUG_GLSL
#define UBONESDEBUG_GLSL

#include "macros.glsl"
#include "data/bones_debug_data.glsl"

layout(binding = VK_UBONESDEBUG_BINDING, set = 1) uniform UBonesDebug
{
	BonesDebug bones;
};

#endif