#include "imp_vk_macros.glsl"

layout(binding = VK_BINSTANCES_BINDING, set = 0) buffer InstancesData { InstanceData i; } instancesData[];