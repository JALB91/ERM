#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "raster_macros.glsl"
#include "vertex_data.glsl"
#include "ubo.glsl"

void main() {
	gl_Position = ubo.mvp * vec4(inPosition, 1.0);
}
