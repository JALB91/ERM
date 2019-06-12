#pragma once

#include "math/vec.h"

#include <string>

namespace erm {
	
	struct Material
	{
		static Material DEFAULT;
		
		std::string mPath;
		std::string mName;
		math::vec3 mAmbient;
		math::vec3 mDiffuse;
		math::vec3 mSpecular;
		float mShininess;
	};
	
}
