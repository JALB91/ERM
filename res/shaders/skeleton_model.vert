#version 330 core

const int MAX_BONES = 50;
const int MAX_WEIGHTS = 4;

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
	vec4 localPos = vec4(aPos, 1.0);
	vec4 localNormal = vec4(aNormal, 0.0);

	for (int i = 0; i < MAX_WEIGHTS; ++i)
	{
		mat4 boneTransform = u_BoneTransforms[boneIds[i]];

		vec4 posePosition = boneTransform * vec4(aPos, 1.0);
		localPos += posePosition * boneWeights[i];

		vec4 worldNormal = boneTransform * vec4(aNormal, 0.0);
		localNormal += worldNormal * boneWeights[i];
	}

	localPos = u_Model * localPos;
	localNormal = u_Model * localNormal;

	FragPos = vec3(localPos);
	Normal = vec3(localNormal);
	gl_Position = u_ViewProjection * localPos;
}
