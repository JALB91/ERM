#pragma once

#include <erm/assets/enums/TextureType.h>

#include <erm/utils/StringID.h>

#include <map>

namespace erm {

struct BindingConfigs
{
	virtual ~BindingConfigs() = default;

	bool isBindingLevelCompatible(const BindingConfigs& other) const;

	bool isMaterialCompatible(const BindingConfigs& other) const;
	bool areTexturesCompatible(const BindingConfigs& other) const;
	
	StringID getTexture(TextureType type) const;

	StringID mMaterial = StringID::INVALID;
	std::map<TextureType, StringID> mTexturesMaps;
};

} // namespace erm
