#ifndef UPBMATERIAL_GLSL
#define UPBMATERIAL_GLSL

#include "macros.glsl"
#include "data/pbmaterial_data.glsl"

layout(binding = VK_UPBMAT_BINDING, set = 1) uniform UPBMaterial
{
    PBMaterial pbMaterial;
};

#endif