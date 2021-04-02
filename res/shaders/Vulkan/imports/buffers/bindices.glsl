#ifndef BINDICES_GLSL
#define BINDICES_GLSL

#include "macros.glsl"

layout(binding = VK_BINDICES_BINDING, set = 0) buffer BIndices { uint i[]; } indices[];

#endif