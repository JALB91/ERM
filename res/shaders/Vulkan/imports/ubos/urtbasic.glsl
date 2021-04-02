#ifndef URTBASIC_GLSL
#define URTBASIC_GLSL

#include "macros.glsl"
#include "data/rtbasic_data.glsl"

layout(binding = VK_URTBASIC_BINDING, set = 1) uniform URTBasic
{
    RTBasic rtBasic;
};

#endif