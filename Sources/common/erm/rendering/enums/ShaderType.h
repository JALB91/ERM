#pragma once

namespace erm {

	enum class ShaderType
	{
		VERTEX,
		FRAGMENT,
		RT_ANY_HIT,
		RT_CLOSEST_HIT,
		RT_RAY_GEN,
		RT_MISS,
		RT_CALLABLE,
		RT_INTERSECTION,
		COUNT
	};

}