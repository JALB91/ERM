#include "erm/rendering/data_structs/BindingConfigs.h"

#include "erm/rendering/materials/Material.h"
#include "erm/rendering/materials/PBMaterial.h"

namespace erm {

BindingConfigs::BindingConfigs()
	: mMaterial(MaterialType::PBR, &PBMaterial::DEFAULT)
{}

bool BindingConfigs::IsBindingLevelCompatible(const BindingConfigs& other) const
{
	return IsMaterialCompatible(other) && AreTexturesCompatible(other);
}

Texture* BindingConfigs::GetTexture(TextureType type) const
{
	{
		auto it = mTexturesMaps.find(type);
		if (it != mTexturesMaps.end() && it->second)
			return it->second;
	}

	if (mMaterial.mType == MaterialType::LEGACY)
	{
		Material* material = static_cast<Material*>(mMaterial.mData);
		auto it = material->mTexturesMaps.find(type);
		if (it != material->mTexturesMaps.end() && it->second)
			return it->second;
	}

	return nullptr;
}

bool BindingConfigs::IsMaterialCompatible(const BindingConfigs& other) const
{
	return mMaterial.mData == other.mMaterial.mData;
}

bool BindingConfigs::AreTexturesCompatible(const BindingConfigs& other) const
{
	for (int i = 0; i < static_cast<int>(TextureType::COUNT); ++i)
	{
		auto it1 = mTexturesMaps.find(static_cast<TextureType>(i));
		auto it2 = other.mTexturesMaps.find(static_cast<TextureType>(i));

		if (it1 != mTexturesMaps.end() && it2 != other.mTexturesMaps.end())
		{
			if (it1->second != it2->second)
				return false;
		}
		else if (it1 != mTexturesMaps.end() || it2 != other.mTexturesMaps.end())
		{
			return false;
		}
	}

	return true;
}

} // namespace erm
