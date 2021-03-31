#include "imp_vk_macros.glsl"

layout(binding = VK_ULIGHT_BINDING, set = 1) uniform Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} light;