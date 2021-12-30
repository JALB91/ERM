#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable

#include "data/payload_data.glsl"
#include "ubos/ucubemap.glsl"
#include "ubos/udepth_sampler.glsl"

layout(location = 0) rayPayloadInEXT HitPayload prd;

void main()
{
    const vec2 p2 = vec2(float(gl_LaunchIDEXT.x) / float(gl_LaunchSizeEXT.x), 1.0 - float(gl_LaunchIDEXT.y) / (gl_LaunchSizeEXT.y));
    vec4 depth = texture(depthSampler, p2);

	prd.mSkip = prd.mSkip || (prd.mDepth == 0 && depth.x != 1.0);
    prd.mHitValue = vec3(texture(cubeMapSampler, gl_WorldRayDirectionEXT));
}
