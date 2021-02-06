#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_BONES = 100;

layout(binding = 0) uniform Skeleton {
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 bonesTransforms[MAX_BONES];
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inBoneWeights;
layout(location = 4) in ivec4 inBoneIds;
layout(location = 5) in int inBoneNum;

layout(location = 0) out vec3 outFragPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outBoneWeights;

void main()
{
	mat4 boneTransform = mat4(1.0 - min(1.0, inBoneNum));

	for (int i = 0; i < inBoneNum; ++i)
	{
		boneTransform += ubo.bonesTransforms[inBoneIds[i]] * inBoneWeights[i];
	}

	boneTransform = ubo.model * boneTransform;

	vec4 localPos = boneTransform * vec4(inPosition, 1.0);
	vec4 localNormal = boneTransform * vec4(inNormal, 0.0);

	outFragPos = vec3(localPos);
	outNormal = vec3(localNormal);
	outBoneWeights = inBoneWeights;
	gl_Position = ubo.projection * ubo.view * localPos;
}
