#ifndef UDEPTH_GLSL
#define UDEPTH_GLSL

#include "macros.glsl"

layout(binding = VK_UDEPTH_BINDING, set = 0, rgba32f) uniform image2D depth;

#endif