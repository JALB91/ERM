#include "imp_vk_macros.glsl"

layout(binding = VK_UVIEW_BINDING, set = 1) uniform View {
	vec3 position;
} view;