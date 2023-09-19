#include "erm/rendering/data_structs/BindingConfigs.h"

#include "erm/assets/materials/Material.h"
#include "erm/assets/materials/PBMaterial.h"

namespace erm {

bool BindingConfigs::isBindingLevelCompatible(const BindingConfigs& other) const
{
	return isMaterialCompatible(other) && areTexturesCompatible(other);
}

bool BindingConfigs::isMaterialCompatible(const BindingConfigs& other) const
{
	return mMaterial == other.mMaterial;
}

bool BindingConfigs::areTexturesCompatible(const BindingConfigs& other) const
{
	for (int i = 0; i < static_cast<int>(TextureType::COUNT); ++i)
	{
		if (getTexture(static_cast<TextureType>(i)) != other.getTexture(static_cast<TextureType>(i)))
		{
			return false;
		}
	}
	
	return true;
}

StringID BindingConfigs::getTexture(TextureType type) const
{
	auto it = mTexturesMaps.find(type);
	if (it != mTexturesMaps.end())
	{
		return it->second;
	}

	return StringID::INVALID;
}

} // namespace erm
