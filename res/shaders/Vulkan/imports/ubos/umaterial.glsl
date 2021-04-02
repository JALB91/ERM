#ifndef UMATERIAL_GLSL
#define UMATERIAL_GLSL

#include "macros.glsl"
#include "data/material_data.glsl"

layout(binding = VK_UMATERIAL_BINDING, set = 1) uniform UMaterial
{ 
	Material material; 
};

#endif