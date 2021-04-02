#ifndef UVIEW_GLSL
#define UVIEW_GLSL

#include "macros.glsl"
#include "data/view_data.glsl"

layout(binding = VK_UVIEW_BINDING, set = 1) uniform UView
{
	View view;
};

#endif