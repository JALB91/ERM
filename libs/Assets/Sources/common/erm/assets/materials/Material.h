#pragma once

#include "erm/assets/enums/TextureType.h"

#include <erm/math/Types.h>

#include <erm/utils/StringID.h>

#include <unordered_map>

namespace erm {

struct Material
{
	Material()
		: mShininess(0.0f)
	{}
	
	std::unordered_map<TextureType, StringID> mTexturesMaps;
	vec3 mAmbient;
	vec3 mDiffuse;
	vec3 mSpecular;
	float mShininess;
};

} // namespace erm
