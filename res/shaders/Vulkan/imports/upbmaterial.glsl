#include "macros.glsl"

layout(binding = VK_UPBMAT_BINDING, set = 1) uniform PBMaterial {
    vec3 albedo;
	float metallic;
	float roughness;
	float ao;
} pbMaterial;