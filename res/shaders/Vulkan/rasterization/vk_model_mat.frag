#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "ubos/umaterial.glsl"
#include "ubos/ulight.glsl"
#include "ubos/uview.glsl"
#include "raster_utils.glsl"

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 n = normalize(Normal);
	vec3 v = normalize(view.position - FragPos);
	outColor = vec4(material.ambient, 1.0);

	vec3 l = normalize(light.position - FragPos);
	float Ndl = clamp(dot(n, l), 0.0, 1.0);

	float spec = pow(max(dot(n, l), 0.0), material.shininess);
	vec3 specular = light.specular * material.specular * spec;

	outColor.rgb += Ndl * light.ambient * lit(l, n, v, material.diffuse, light.diffuse);
	outColor.rgb += specular;
}
