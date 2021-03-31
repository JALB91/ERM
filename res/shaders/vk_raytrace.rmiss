#version 460
#extension GL_EXT_ray_tracing : require

struct hitPayload
{
    vec3 hitValue;
    vec3 atten;
    vec3 origin;
    vec3 dir;
    int depth;
    int done;
};

layout(binding = 20, set = 0) uniform samplerCube cubeMap;

layout(location = 0) rayPayloadInEXT hitPayload prd;

void main()
{
    prd.hitValue = vec3(texture(cubeMap, gl_WorldRayDirectionEXT));
}