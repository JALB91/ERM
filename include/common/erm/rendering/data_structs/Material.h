#pragma once

#include "erm/math/vec.h"

#include <string>

namespace erm {
	class Texture;
}

namespace erm {

	struct Material
	{
		static Material DEFAULT;

		Material() = delete;

		std::string mPath;
		std::string mName;
		math::vec3 mAmbient;
		math::vec3 mDiffuse;
		math::vec3 mSpecular;
		float mShininess;
		Texture* mDiffuseMap;
		Texture* mNormalMap;
	};

} // namespace erm
