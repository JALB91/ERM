#pragma once

#include "erm/rendering/enums/MaterialType.h"

namespace erm {

class MaterialHandle
{
public:
	MaterialHandle();
	MaterialHandle(MaterialType materialType);
	MaterialHandle(MaterialType materialType, void* data);

	MaterialType mType;
	void* mData;
};

} // namespace erm
