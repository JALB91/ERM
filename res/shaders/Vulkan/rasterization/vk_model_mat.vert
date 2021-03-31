#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "imp_vk_raster_macros.glsl"
#include "imp_vk_vertex_data.glsl"
#include "imp_vk_umvp.glsl"

layout(location = 0) out vec3 outFragPos;
layout(location = 1) out vec3 outNormal;

void main()
{
	outFragPos = vec3(mvp.model * vec4(inPosition, 1.0));
	outNormal = mat3(mvp.model) * inNormal;
	gl_Position = mvp.projection * mvp.view * vec4(outFragPos, 1.0);
}
