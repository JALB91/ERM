#pragma once

#include "erm/assets/enums/TextureType.h"

#include <erm/utils/StringID.h>

#include <erm/math/Types.h>

#include <unordered_map>

namespace erm {

struct PBMaterial
{
	std::unordered_map<TextureType, StringID> mTexturesMap;
	vec3 mAlbedo = vec3(0.0f);
	float mMetallic = 0.0f;
	float mRoughness = 0.0f;
	float mAO = 0.0f;
};

} // namespace erm
