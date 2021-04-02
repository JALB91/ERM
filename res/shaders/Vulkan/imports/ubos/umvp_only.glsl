#ifndef UMVP_ONLY_GLSL
#define UMVP_ONLY_GLSL

#include "macros.glsl"
#include "data/mvp_only_data.glsl"

layout(binding = VK_UBO_BINDING, set = 1) uniform UMVPOnly
{
	MVPOnly mvpOnly;
};

#endif