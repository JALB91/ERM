#ifndef PBMATERIAL_DATA_GLSL
#define PBMATERIAL_DATA_GLSL

struct PBMaterial 
{
    vec3 albedo;
	float metallic;
	float roughness;
	float ao;
};

#endif