#pragma once

#include <erm/resources/textures/TextureType.h>
#include <erm/resources/materials/MaterialHandle.h>

#include <map>

namespace erm {
class Texture;
} // namespace erm

namespace erm {

struct BindingConfigs
{
	BindingConfigs();
	virtual ~BindingConfigs() = default;

	bool IsBindingLevelCompatible(const BindingConfigs& other) const;

	bool IsMaterialCompatible(const BindingConfigs& other) const;
	bool AreTexturesCompatible(const BindingConfigs& other) const;
	
	Texture* GetTexture(TextureType type) const;

	MaterialHandle mMaterial;
	std::map<TextureType, Texture*> mTexturesMaps;
};

} // namespace erm
