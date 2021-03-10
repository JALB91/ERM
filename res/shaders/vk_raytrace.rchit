#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_nonuniform_qualifier : enable

struct Vertex
{
	vec3 pos;
	vec3 nrm;
	vec2 texCoord;
	vec4 mBoneWeights;
	ivec4 mBoneIds;
	int mBoneNum;
	int mDebugBoneId;
};

struct InstanceData
{
	mat4 transform;
	mat4 transformIT;
};

hitAttributeEXT vec2 attribs;

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 1, set = 0, rgba32f) uniform image2D image;
layout(binding = 2, set = 0, rgba32f) uniform image2D depth;
layout(binding = 5, set = 0, scalar) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 6, set = 0) buffer Indices { uint i[]; } indices[];
layout(binding = 7, set = 0) buffer InstancesData { InstanceData i; } instancesData[];
layout(binding = 8, set = 1) uniform Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} light;

layout(location = 0) rayPayloadInEXT vec4 hitValue;
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
	vec3 normal = v0.nrm * barycentrics.x + v1.nrm * barycentrics.y + v2.nrm * barycentrics.z;
	
	// Transforming the normal to world space
	normal = normalize(vec3(instancesData[gl_InstanceCustomIndexEXT].i.transformIT * vec4(normal, 0.0)));

	// Computing the coordinates of the hit position
	vec3 worldPos = v0.pos * barycentrics.x + v1.pos * barycentrics.y + v2.pos * barycentrics.z;
	
	// Transforming the position to world space
	worldPos = vec3(instancesData[gl_InstanceCustomIndexEXT].i.transform * vec4(worldPos, 1.0));

	vec3 lDir = light.position - worldPos;
	float dist = length(lDir);
	vec3 L = normalize(lDir);
	float dotNL = max(dot(normal, L), 0.1);

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

	hitValue.rgb = vec3(dotNL);
}
