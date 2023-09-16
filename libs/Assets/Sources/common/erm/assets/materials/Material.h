#pragma once

#include "erm/assets/enums/TextureType.h"

#include <erm/math/vec.h>

#include <erm/utils/StringID.h>

#include <unordered_map>

namespace erm {

struct Material
{
	Material()
		: mShininess(0.0f)
	{}
	
	std::unordered_map<TextureType, StringID> mTexturesMaps;
	math::vec3 mAmbient;
	math::vec3 mDiffuse;
	math::vec3 mSpecular;
	float mShininess;
};

} // namespace erm
