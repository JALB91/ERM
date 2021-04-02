#ifndef BONES_DEBUG_DATA_GLSL
#define BONES_DEBUG_DATA_GLSL

struct BonesDebug 
{
	mat4 bonesModels[MAX_BONES];
	mat4 model;
	mat4 view;
	mat4 projection;
};

#endif