#pragma once

#include "erm/rendering/enums/TextureType.h"

#include <map>

namespace erm {
	class Texture;
	struct Material;
	struct PBMaterial;
} // namespace erm

namespace erm {

	struct BindingConfigs
	{
		static const BindingConfigs MODELS_BINDING_CONFIGS;

		BindingConfigs();
		virtual ~BindingConfigs() = default;

		Texture* GetTexture(TextureType type) const;

		bool IsBindingLevelCompatible(const BindingConfigs& other) const;

		bool IsMaterialCompatible(const BindingConfigs& other) const;
		bool AreTexturesCompatible(const BindingConfigs& other) const;

		PBMaterial* mPBMaterial;
		Material* mMaterial;
		std::map<TextureType, Texture*> mTexturesMaps;
	};

} // namespace erm