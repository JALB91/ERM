#pragma once

#include "erm/assets/enums/TextureType.h"

#include <erm/math/Types.h>

#include <erm/utils/StringID.h>

#include <unordered_map>

namespace erm {

struct Material
{
	std::unordered_map<TextureType, StringID> mTexturesMaps;
	vec3 mAmbient = vec3(0.0f);
	vec3 mDiffuse = vec3(0.0f);
	vec3 mSpecular = vec3(0.0f);
	float mShininess = 0.0f;
};

} // namespace erm
