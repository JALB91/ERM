#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} material;

layout(binding = 3) uniform Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} light;

layout(binding = 4) uniform View {
	vec3 position;
} view;

layout(binding = 5) uniform sampler2D diffuseSampler;
layout(binding = 6) uniform sampler2D specularSampler;

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
	// texture(diffuseSampler, TexCoord);

	// ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(diffuseSampler, TexCoord)));
    
    // specular
    vec3 viewDir = normalize(view.position - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(specularSampler, TexCoord)));  
        
    vec3 result = ambient + diffuse + specular;
    outColor = vec4(result, 1.0);
}
