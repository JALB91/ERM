#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable

layout(binding = 1, set = 0, rgba32f) uniform image2D image;
layout(binding = 2, set = 0, rgba32f) uniform image2D depth;

layout(location = 0) rayPayloadInEXT vec4 hitValue;

void main()
{
	hitValue = vec4(0.2, 0.5, 0.5, 1.0);
}
