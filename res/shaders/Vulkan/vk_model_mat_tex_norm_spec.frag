#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "imports/imp_vk_umaterial.glsl"
#include "imports/imp_vk_ulight.glsl"
#include "imports/imp_vk_uview.glsl"
#include "imports/imp_vk_udiffuse.glsl"
#include "imports/imp_vk_uspecular.glsl"

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

layout(location = 0) out vec4 outColor;

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
