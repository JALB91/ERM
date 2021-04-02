#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "raster_macros.glsl"
#include "data/vertex_data.glsl"
#include "ubos/umvp_only.glsl"

void main() {
	gl_Position = mvpOnly.mvp * vec4(inPosition, 1.0);
}
