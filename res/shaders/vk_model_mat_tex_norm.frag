#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} material;

layout(binding = 2) uniform Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} light;

layout(binding = 3) uniform View {
	vec3 position;
} view;

layout(binding = 4) uniform sampler2D diffuseSampler;
layout(binding = 5) uniform sampler2D normalSampler;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

layout(location = 0) out vec4 outColor;

vec3 lit(vec3 l, vec3 n, vec3 v) {
	vec3 r_l = reflect(-l, n);
	float s = clamp(100.0 * dot(r_l, v) - 100.0, 0.0, 1.0);
	return mix(material.diffuse, light.diffuse, s);
}

void main()
{
	outColor = texture(diffuseSampler, TexCoord);

	vec3 n = normalize(vec3(texture(normalSampler, TexCoord)));
	vec3 v = normalize(view.position - FragPos);
	vec3 l = normalize(light.position - FragPos);

	float Ndl = clamp(dot(n, l), 0.0, 1.0);

	float spec = pow(max(dot(n, l), 0.0), material.shininess);
	vec3 specular = light.specular * material.specular * spec;

	outColor.rgb += Ndl * light.ambient * lit(l, n, v);
	outColor.rgb += specular;
}
