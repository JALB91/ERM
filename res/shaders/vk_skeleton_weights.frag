#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec4 BoneWeights;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = BoneWeights;
}
