#ifndef SKELETON_DATA_GLSL
#define SKELETON_DATA_GLSL

#include "macros.glsl"

struct Skeleton
{
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 bonesTransforms[MAX_BONES];
};

#endif