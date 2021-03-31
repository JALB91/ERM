#include "macros.glsl"
#include "rtx_macros.glsl"
#include "vertex_data.glsl"

layout(binding = VK_BVERTICES_BINDING, set = 0, scalar) buffer Vertices { Vertex v[]; } vertices[];