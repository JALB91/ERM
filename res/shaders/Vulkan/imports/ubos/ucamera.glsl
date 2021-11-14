#ifndef UCAMERA_GLSL
#define UCAMERA_GLSL

#include "macros.glsl"
#include "data/camera_data.glsl"

layout(binding = VK_UCAMERA_BINDING, set = 1) uniform UCamera
{
    Camera camera;
};

#endif