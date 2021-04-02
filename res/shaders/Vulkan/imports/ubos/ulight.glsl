#ifndef ULIGHT_GLSL
#define ULIGHT_GLSL

#include "macros.glsl"
#include "data/light_data.glsl"

layout(binding = VK_ULIGHT_BINDING, set = 1) uniform ULight
{
	Light light;
};

#endif