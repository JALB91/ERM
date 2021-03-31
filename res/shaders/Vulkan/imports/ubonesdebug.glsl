#include "macros.glsl"

layout(binding = VK_UBONESDEBUG_BINDING, set = 1) uniform BonesDebug {
	mat4 bonesModels[MAX_BONES];
	mat4 model;
	mat4 view;
	mat4 projection;
} bones;