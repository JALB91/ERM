#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_BONES = 100;

layout(binding = 0) uniform Skeleton {
	mat4 model;
	mat4 viewProj;
	mat4 bonesTransforms[MAX_BONES];
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inBoneWeights;
layout(location = 4) in ivec4 inBoneIds;

layout(location = 0) out vec3 outFragPos;
layout(location = 1) out vec3 outNormal;

void main()
{
	mat4 boneTransform = ubo.bonesTransforms[inBoneIds[0]] * inBoneWeights[0];
	boneTransform += ubo.bonesTransforms[inBoneIds[1]] * inBoneWeights[1];
	boneTransform += ubo.bonesTransforms[inBoneIds[2]] * inBoneWeights[2];
	boneTransform += ubo.bonesTransforms[inBoneIds[3]] * inBoneWeights[3];
	boneTransform = ubo.model * boneTransform;

	vec4 localPos = boneTransform * vec4(inPosition, 1.0);
	vec4 localNormal = boneTransform * vec4(inNormal, 0.0);

	outFragPos = vec3(localPos);
	outNormal = vec3(localNormal);
	gl_Position = ubo.viewProj * localPos;
}
