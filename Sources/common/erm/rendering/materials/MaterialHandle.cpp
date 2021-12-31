#include "erm/rendering/materials/MaterialHandle.h"
#include "erm/rendering/materials/Material.h"
#include "erm/rendering/materials/PBMaterial.h"

#include "erm/utils/Utils.h"

namespace erm {

MaterialHandle::MaterialHandle()
	: MaterialHandle(MaterialType::PBR)
{}

MaterialHandle::MaterialHandle(MaterialType materialType)
	: mType(materialType)
{
	switch (mType)
	{
		default:
			ERM_ASSERT(false);
		case MaterialType::LEGACY:
			mData = &Material::DEFAULT;
			break;
		case MaterialType::PBR:
			mData = &PBMaterial::DEFAULT;
			break;
	}
}

MaterialHandle::MaterialHandle(MaterialType materialType, void* data)
	: mType(materialType)
	, mData(data)
{}

} // namespace erm
