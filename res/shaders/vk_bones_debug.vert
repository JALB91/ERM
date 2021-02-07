#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_BONES 100

layout(binding = 0) uniform BonesDebug {
	mat4 bonesModels[MAX_BONES];
	mat4 model;
	mat4 view;
	mat4 projection;
} bones;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in int inDebugBoneId;

void main()
{
	gl_Position = bones.projection * bones.view * bones.model * bones.bonesModels[inDebugBoneId] * vec4(inPosition, 1.0);
}
