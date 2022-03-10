#ifndef UDEPTH_BUFFER_GLSL
#define UDEPTH_BUFFER_GLSL

#include "macros.glsl"

layout(binding = VK_UDEPTH_BUFFER_BINDING, set = 0, rgba32f) uniform image2D depthBuffer;

#endif