#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec4 BoneWeights;
layout(location = 3) flat in ivec4 BoneIds;
layout(location = 4) flat in int BoneNum;

layout(location = 0) out vec4 outColor;

void main()
{
//	outColor = BoneWeights;
	outColor = vec4(0.0);
	outColor.a = 1.0;
	switch (BoneIds.w) {
		case 0:
			outColor.r = 1.0;
			break;
		case 1:
			outColor.g = 1.0;
			break;
		case 2:
			outColor.b = 1.0;
			break;
		case 3:
			outColor.rgb = vec3(0.5);
			break;
	}
	outColor.rgb = vec3(0.0);

	switch (BoneNum) {
		default:
		case 4:
			outColor.rgb += vec3(BoneWeights[3]);
		case 3:
			outColor.b += BoneWeights[2];
		case 2:
			outColor.g += BoneWeights[1];
		case 1:
			outColor.r += BoneWeights[0];
		case 0:
			break;
	}
}
