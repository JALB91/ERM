#include "macros.glsl"

layout(binding = VK_UBO_BINDING, set = 1) uniform UniformBufferObject {
	mat4 mvp;
} ubo;