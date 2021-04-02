#ifndef UPBLIGHT_GLSL
#define UPBLIGHT_GLSL

#include "macros.glsl"
#include "data/pblight_data.glsl"

layout(binding = VK_UPBLIGHT_BINDING, set = 1) uniform UPBLight 
{
	PBLight pbLight;
};

#endif