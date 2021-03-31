#include "macros.glsl"

layout(binding = VK_URTBASIC_BINDING, set = 1) uniform UboRTBasic {
    mat4 projection;
	mat4 view;
} rtBasic;