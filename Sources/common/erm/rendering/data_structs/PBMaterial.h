#pragma once

#include "erm/math/vec.h"

#include "erm/rendering/data_structs/IAsset.h"

namespace erm {

struct PBMaterial : public IAsset
{
	static PBMaterial DEFAULT;

	PBMaterial(
		const char* path,
		const char* name,
		const math::vec3& albedo,
		float metallic,
		float roughness,
		float ao)
		: IAsset(path, name)
		, mAlbedo(albedo)
		, mMetallic(metallic)
		, mRoughness(roughness)
		, mAO(ao)
	{}

	math::vec3 mAlbedo;
	float mMetallic;
	float mRoughness;
	float mAO;
};

} // namespace erm