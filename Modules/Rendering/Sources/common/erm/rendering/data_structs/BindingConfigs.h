#pragma once

#include <erm/assets/enums/TextureType.h>

#include <erm/utils/StringID.h>

#include <map>

namespace erm {

struct BindingConfigs
{
	virtual ~BindingConfigs() = default;

	bool IsBindingLevelCompatible(const BindingConfigs& other) const;

	bool IsMaterialCompatible(const BindingConfigs& other) const;
	bool AreTexturesCompatible(const BindingConfigs& other) const;
	
	StringID GetTexture(TextureType type) const;

	StringID mMaterial = StringID::INVALID;
	std::map<TextureType, StringID> mTexturesMaps;
};

} // namespace erm
