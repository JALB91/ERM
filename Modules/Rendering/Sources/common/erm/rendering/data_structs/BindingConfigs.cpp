#include "erm/rendering/data_structs/BindingConfigs.h"

#include "erm/resources/materials/Material.h"
#include "erm/resources/materials/PBMaterial.h"

namespace erm {

BindingConfigs::BindingConfigs()
	: mMaterial(MaterialType::PBR, &PBMaterial::DEFAULT)
{}

bool BindingConfigs::IsBindingLevelCompatible(const BindingConfigs& other) const
{
	return IsMaterialCompatible(other) && AreTexturesCompatible(other);
}

bool BindingConfigs::IsMaterialCompatible(const BindingConfigs& other) const
{
	return mMaterial.mData == other.mMaterial.mData;
}

bool BindingConfigs::AreTexturesCompatible(const BindingConfigs& other) const
{
	for (int i = 0; i < static_cast<int>(TextureType::COUNT); ++i)
	{
		if (GetTexture(static_cast<TextureType>(i)) != other.GetTexture(static_cast<TextureType>(i)))
			return false;
	}
	
	return true;
}

Texture* BindingConfigs::GetTexture(TextureType type) const
{
	if (mMaterial.mType == MaterialType::LEGACY)
	{
		Material* material = static_cast<Material*>(mMaterial.mData);
		auto it = material->mTexturesMaps.find(type);
		if (it != material->mTexturesMaps.end() && it->second)
			return it->second;
	}
	
	{
		auto it = mTexturesMaps.find(type);
		if (it != mTexturesMaps.end() && it->second)
			return it->second;
	}

	return nullptr;
}

} // namespace erm
