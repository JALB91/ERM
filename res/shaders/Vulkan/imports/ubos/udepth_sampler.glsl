#ifndef UDEPTH_SAMPLER_GLSL
#define UDEPTH_SAMPLER_GLSL

#include "macros.glsl"

layout(binding = VK_UDEPTH_SAMPLER_BINDING, set = 0) uniform sampler2D depthSampler;

#endif