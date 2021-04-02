#include "erm/rendering/data_structs/MaterialHandle.h"
#include "erm/rendering/data_structs/Material.h"
#include "erm/rendering/data_structs/PBMaterial.h"

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
			ASSERT(false);
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
