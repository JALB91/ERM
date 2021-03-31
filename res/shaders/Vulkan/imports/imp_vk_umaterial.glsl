#include "imp_vk_macros.glsl"

layout(binding = VK_UMATERIAL_BINDING, set = 1) uniform Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} material;