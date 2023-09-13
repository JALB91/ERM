#include "erm/rendering/data_structs/BindingConfigs.h"

#include "erm/assets/materials/Material.h"
#include "erm/assets/materials/PBMaterial.h"

namespace erm {

bool BindingConfigs::IsBindingLevelCompatible(const BindingConfigs& other) const
{
	return IsMaterialCompatible(other) && AreTexturesCompatible(other);
}

bool BindingConfigs::IsMaterialCompatible(const BindingConfigs& other) const
{
	return mMaterial == other.mMaterial;
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

StringID BindingConfigs::GetTexture(TextureType type) const
{
	auto it = mTexturesMaps.find(type);
	if (it != mTexturesMaps.end())
		return it->second;

	return StringID::INVALID;
}

} // namespace erm
