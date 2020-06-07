#pragma once

#include "erm/rendering/data_structs/UniformBufferObject.h"

namespace erm {

	struct UboData
	{
		UboId mUboId;
		size_t mSize;
		uint32_t mOffset;
		uint32_t mBinding;
	};

} // namespace erm
