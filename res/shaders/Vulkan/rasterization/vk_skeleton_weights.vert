#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "imp_vk_raster_macros.glsl"
#include "imp_vk_vertex_data.glsl"
#include "imp_vk_uskeleton.glsl"

layout(location = 0) out vec3 outFragPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outBoneWeights;
layout(location = 3) flat out ivec4 outBoneIds;
layout(location = 4) flat out int outBoneNum;

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
	outBoneIds = inBoneIds;
	outBoneNum = inBoneNum;
	gl_Position = ubo.projection * ubo.view * localPos;
}
