#version 460
#extension GL_EXT_ray_tracing : require

layout(binding = 1, set = 0, rgba32f) uniform image2D image;

layout(location = 0) rayPayloadInEXT vec4 hitValue;

void main()
{
    const ivec2 p = ivec2(gl_LaunchIDEXT.x, gl_LaunchSizeEXT.y-gl_LaunchIDEXT.y);
    hitValue = imageLoad(image, p);
}