#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable

#include "buffers/bvertices.glsl"
#include "buffers/bindices.glsl"
#include "buffers/binstances.glsl"
#include "data/instance_data.glsl"
#include "data/payload_data.glsl"
#include "ubos/utlas.glsl"
#include "ubos/uimage.glsl"
#include "ubos/udepth.glsl"
#include "ubos/ulight.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT HitPayload payload;
layout(location = 1) rayPayloadEXT bool isShadowed;

void main()
{
	// Indices of the triangle
	ivec3 ind = ivec3(
		indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 0],   //
		indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 1],   //
		indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 2]);  //

	// Vertex of the triangle
	Vertex v0 = vertices[gl_InstanceCustomIndexEXT].v[ind.x];
	Vertex v1 = vertices[gl_InstanceCustomIndexEXT].v[ind.y];
	Vertex v2 = vertices[gl_InstanceCustomIndexEXT].v[ind.z];

	const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

	// Computing the normal at hit position
	vec3 normal = v0.mNormal * barycentrics.x + v1.mNormal * barycentrics.y + v2.mNormal * barycentrics.z;
	
	// Transforming the normal to world space
	normal = normalize(vec3(instancesData[gl_InstanceCustomIndexEXT].i.transformIT * vec4(normal, 0.0)));

	// Computing the coordinates of the hit position
	vec3 worldPos = v0.mPosition * barycentrics.x + v1.mPosition * barycentrics.y + v2.mPosition * barycentrics.z;
	
	// Transforming the position to world space
	worldPos = vec3(instancesData[gl_InstanceCustomIndexEXT].i.transform * vec4(worldPos, 1.0));

	vec3 lDir = light.position - worldPos;
	float dist = length(lDir);
	float intensity = 10.0 / (dist * dist);
	vec3 L = normalize(lDir);
	float dotNL = clamp(dot(normal, L), 0.0, 1.0);

	// Tracing shadow ray only if the light is visible from the surface
	if (dot(normal, L) > 0)
	{
		float tMin   = 0.1;
		float tMax   = dist;
		vec3  origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
		vec3  rayDir = L;
		uint  flags  = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
		isShadowed = true;
		traceRayEXT(
			topLevelAS,  // acceleration structure
			flags,       // rayFlags
			0xFF,        // cullMask
			0,           // sbtRecordOffset
			0,           // sbtRecordStride
			1,           // missIndex
			origin,      // ray origin
			tMin,        // ray min range
			rayDir,      // ray direction
			tMax,        // ray max range
			1            // payload (location = 1)
		);

		if (isShadowed)
		{
			dotNL *= 0.5;
		}
	}

	if (gl_InstanceCustomIndexEXT >= 20)
	{
		payload.mDone = 0;
	}

	payload.mOrigin = worldPos;
	payload.mDir = reflect(gl_WorldRayDirectionEXT, normal);
	payload.mHitValue.rgb *= vec3(dotNL * intensity);
}
