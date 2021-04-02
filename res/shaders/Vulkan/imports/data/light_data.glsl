#ifndef LIGHT_DATA_GLSL
#define LIGHT_DATA

struct Light 
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#endif