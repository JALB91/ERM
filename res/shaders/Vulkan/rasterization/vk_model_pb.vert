#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "raster_macros.glsl"
#include "data/vertex_data.glsl"
#include "ubos/umvp.glsl"

layout(location = 0) out vec3 outFragPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord;

void main()
{
	outFragPos = vec3(mvp.model * vec4(inPosition, 1.0));
	outNormal = mat3(mvp.model) * inNormal;
	gl_Position = mvp.projection * mvp.view * vec4(outFragPos, 1.0);
	outTexCoord = inTexCoord;
}
