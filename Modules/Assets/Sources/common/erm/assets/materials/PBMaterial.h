#pragma once

#include "erm/assets/enums/TextureType.h"

#include <erm/utils/StringID.h>

#include <erm/math/vec.h>

#include <unordered_map>

namespace erm {

struct PBMaterial
{
	PBMaterial()
		: mMetallic(0.0f)
		, mRoughness(0.0f)
		, mAO(0.0f)
	{}

	std::unordered_map<TextureType, StringID> mTexturesMap;
	math::vec3 mAlbedo;
	float mMetallic;
	float mRoughness;
	float mAO;
};

} // namespace erm
