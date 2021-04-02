#ifndef BINSTANCES_GLSL
#define BINSTANCES_GLSL

#include "macros.glsl"
#include "data/instance_data.glsl"

layout(binding = VK_BINSTANCES_BINDING, set = 0) buffer BInstancesData { InstanceData i; } instancesData[];

#endif