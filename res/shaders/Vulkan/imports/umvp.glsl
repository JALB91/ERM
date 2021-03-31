#include "macros.glsl"

layout(binding = VK_UMVP_BINDING, set = 1) uniform ModelViewProj {
	mat4 model;
	mat4 view;
	mat4 projection;
} mvp;