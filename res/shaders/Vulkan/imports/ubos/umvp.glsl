#ifndef UMVP_GLSL
#define UMVP_GLSL

#include "macros.glsl"
#include "data/mvp_data.glsl"

layout(binding = VK_UMVP_BINDING, set = 1) uniform UModelViewProj 
{
	ModelViewProj mvp;
};

#endif