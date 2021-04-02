#ifndef USKELETON_GLSL
#define USKELETON_GLSL

#include "macros.glsl"
#include "data/skeleton_data.glsl"

layout(binding = VK_USKELETON_BINDING, set = 1) uniform USkeleton
{
	Skeleton skeleton;
};

#endif