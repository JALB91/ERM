#pragma once

#include "erm/math/vec.h"

#include <string>

namespace erm {

	struct PBMaterial
	{
		static PBMaterial DEFAULT;

		PBMaterial() = delete;

		std::string mPath;
		std::string mName;
		math::vec3 mAlbedo;
		float mMetallic;
		float mRoughness;
		float mAO;
	};

} // namespace erm