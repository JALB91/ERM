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

hitAttributeEXT vec2 attribs;

layout(binding = 1, set = 0, rgba32f) uniform image2D image;
layout(binding = 2, set = 0, rgba32f) uniform image2D depth;
layout(binding = 5, set = 0, scalar) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 6, set = 0) buffer Indices { uint i[]; } indices[];
layout(binding = 7, set = 0) buffer Transforms { mat4 i; } transforms[];
layout(binding = 8, set = 1) uniform Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} light;

layout(location = 0) rayPayloadInEXT vec4 hitValue;

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
	normal = normalize(vec3(transforms[gl_InstanceCustomIndexEXT].i * vec4(normal, 0.0)));

	// Computing the coordinates of the hit position
	vec3 worldPos = v0.pos * barycentrics.x + v1.pos * barycentrics.y + v2.pos * barycentrics.z;
	
	// Transforming the position to world space
	worldPos = vec3(transforms[gl_InstanceCustomIndexEXT].i * vec4(worldPos, 1.0));

	vec3 L = normalize(light.position - vec3(0));
	float dotNL = max(dot(normal, L), 0.2);

	hitValue.rgb = vec3(dotNL);
}
