#pragma once

namespace erm {

enum class ShaderType
{
	VERTEX,
	FRAGMENT,
#ifdef ERM_RAY_TRACING_ENABLED
	RT_ANY_HIT,
	RT_CLOSEST_HIT,
	RT_RAY_GEN,
	RT_MISS,
	RT_CALLABLE,
	RT_INTERSECTION,
#endif
	COUNT
};

} // namespace erm
