#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

// material parameters
uniform vec3  u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_Ao;

// lights
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light u_Lights[4];

uniform vec3 u_ViewPos;

const float PI = 3.14159265359;
  
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    if(cosTheta>1.0) cosTheta=1.0;
    float p=pow(1.0 - cosTheta, 5.0);
    return F0 + (1.0 - F0) * p;
}

void main()
{		
    vec3 N = normalize(Normal);
    vec3 V = normalize(u_ViewPos - FragPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, u_Albedo, u_Metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);

    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(u_Lights[i].position - FragPos);
        vec3 H = normalize(V + L);
        float distance    = length(u_Lights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = u_Lights[i].diffuse * attenuation;
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, u_Roughness);
        float G   = GeometrySmith(N, V, L, u_Roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - u_Metallic;
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * u_Albedo / PI + u_Lights[i].specular) * radiance * NdotL; 
    }   
  
    vec3 ambient = vec3(0.03) * u_Albedo * u_Ao;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    FragColor = vec4(color, 1.0);
} 