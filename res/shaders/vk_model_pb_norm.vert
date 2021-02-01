#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform ModelViewProj {
	mat4 model;
	mat4 view;
	mat4 projection;
} ubo;

layout(binding = 1) uniform sampler2D normalSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outFragPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord;

void main()
{
	outFragPos = vec3(ubo.model * vec4(inPosition, 1.0));
	outNormal = mat3(ubo.model) * vec3(texture(normalSampler, inTexCoord));
	gl_Position = ubo.projection * ubo.view * vec4(outFragPos, 1.0);
	outTexCoord = inTexCoord;
}
