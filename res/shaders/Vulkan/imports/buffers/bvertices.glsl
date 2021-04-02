#ifndef BVERTICES_GLSL
#define BVERTICES_GLSL

#include "macros.glsl"
#include "rtx_macros.glsl"
#include "data/vertex_data.glsl"

layout(binding = VK_BVERTICES_BINDING, set = 0, scalar) buffer BVertices { Vertex v[]; } vertices[];

#endif