#version 330 core

out vec4 FragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;

uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform Light u_Light;

vec3 lit(vec3 l, vec3 n, vec3 v) {
	vec3 r_l = reflect(-l, n);
	float s = clamp(100.0 * dot(r_l, v) - 100.0, 0.0, 1.0);
	return mix(u_Material.diffuse, u_Light.diffuse, s);
}

void main()
{
	vec3 n = normalize(Normal);
	vec3 v = normalize(u_ViewPos - FragPos);
	FragColor = vec4(u_Material.ambient, 1.0);

	vec3 l = normalize(u_Light.position - FragPos);
	float Ndl = clamp(dot(n, l), 0.0, 1.0);

	float spec = pow(max(dot(n, l), 0.0), u_Material.shininess);
	vec3 specular = u_Light.specular * u_Material.specular * spec;

	FragColor.rgb += Ndl * u_Light.ambient * lit(l, n, v);
	FragColor.rgb += specular;
}
