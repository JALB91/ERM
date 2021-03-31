#include "imp_vk_macros.glsl"

layout(binding = VK_UPBLIGHT_BINDING, set = 1) uniform PBLight {
	vec3 position;
	vec3 color;
} pbLight;