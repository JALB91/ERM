#ifndef UPRESENT_BUFFER_GLSL
#define UPRESENT_BUFFER_GLSL

#include "macros.glsl"

layout(binding = VK_UPRESENT_BUFFER_BINDING, set = 0, rgba32f) uniform image2D presentBuffer;

#endif