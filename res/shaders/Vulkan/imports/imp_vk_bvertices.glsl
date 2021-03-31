#include "imp_vk_macros.glsl"
#include "imp_vk_rtx_macros.glsl"
#include "imp_vk_vertex_data.glsl"

layout(binding = VK_BVERTICES_BINDING, set = 0, scalar) buffer Vertices { Vertex v[]; } vertices[];