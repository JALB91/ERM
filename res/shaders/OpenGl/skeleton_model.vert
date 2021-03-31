#version 330 core

const int MAX_BONES = 100;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec4 boneWeights;
layout (location = 4) in ivec4 boneIds;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform mat4 u_BoneTransforms[MAX_BONES];

void main()
{
	mat4 boneTransform = u_BoneTransforms[boneIds[0]] * boneWeights[0];
	boneTransform += u_BoneTransforms[boneIds[1]] * boneWeights[1];
	boneTransform += u_BoneTransforms[boneIds[2]] * boneWeights[2];
	boneTransform += u_BoneTransforms[boneIds[3]] * boneWeights[3];
	boneTransform = u_Model * boneTransform;

	vec4 localPos = boneTransform * vec4(aPos, 1.0);
	vec4 localNormal = boneTransform * vec4(aNormal, 0.0);

	FragPos = vec3(localPos);
	Normal = vec3(localNormal);
	gl_Position = u_ViewProjection * localPos;
}
