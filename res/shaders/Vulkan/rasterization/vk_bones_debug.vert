#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "raster_macros.glsl"
#include "data/vertex_data.glsl"
#include "ubos/ubonesdebug.glsl"

void main()
{
	gl_Position = bones.projection * bones.view * bones.model * bones.bonesModels[inBoneDebugId] * vec4(inPosition, 1.0);
}
