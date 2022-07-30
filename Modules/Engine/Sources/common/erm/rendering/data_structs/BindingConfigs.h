#pragma once

#include "erm/rendering/enums/TextureType.h"
#include "erm/rendering/materials/MaterialHandle.h"

#include <map>

namespace erm {
class Texture;
} // namespace erm

namespace erm {

struct BindingConfigs
{
	BindingConfigs();
	virtual ~BindingConfigs() = default;

	Texture* GetTexture(TextureType type) const;

	bool IsBindingLevelCompatible(const BindingConfigs& other) const;

	bool IsMaterialCompatible(const BindingConfigs& other) const;
	bool AreTexturesCompatible(const BindingConfigs& other) const;

	MaterialHandle mMaterial;
	std::map<TextureType, Texture*> mTexturesMaps;
};

} // namespace erm
