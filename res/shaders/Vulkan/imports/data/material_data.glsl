#ifndef MATERIAL_DATA_GLSL
#define MATERIAL_DATA_GLSL

struct Material 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

#endif