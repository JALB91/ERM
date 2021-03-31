#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "imports/imp_vk_raster_macros.glsl"
#include "imports/imp_vk_vertex_data.glsl"
#include "imports/imp_vk_ubonesdebug.glsl"

void main()
{
	gl_Position = bones.projection * bones.view * bones.model * bones.bonesModels[inBoneDebugId] * vec4(inPosition, 1.0);
}
