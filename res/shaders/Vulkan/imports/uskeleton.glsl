#include "macros.glsl"

layout(binding = VK_USKELETON_BINDING, set = 1) uniform Skeleton {
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 bonesTransforms[MAX_BONES];
} ubo;