#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable

#include "payload_data.glsl"
#include "ucubemap.glsl"

layout(location = 0) rayPayloadInEXT HitPayload prd;

void main()
{
    prd.mHitValue = vec3(texture(cubeMap, gl_WorldRayDirectionEXT));
}