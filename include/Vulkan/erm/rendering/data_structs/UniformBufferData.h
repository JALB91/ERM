#pragma once

#include "erm/rendering/data_structs/UniformBufferObject.h"
#include "erm/rendering/enums/TextureType.h"

namespace erm {

	struct IBindingData
	{
		IBindingData(uint32_t binding)
			: mBinding(binding)
		{}
		virtual ~IBindingData() = default;

		uint32_t mBinding;
	};

	struct UboData : IBindingData
	{
		UboData(UboId uboId, size_t size, uint32_t offset, uint32_t binding)
			: IBindingData(binding)
			, mUboId(uboId)
			, mSize(size)
			, mOffset(offset)
		{}

		UboId mUboId;
		size_t mSize;
		uint32_t mOffset;
	};

	struct SamplerData : IBindingData
	{
		SamplerData(uint32_t binding, TextureType textureType)
			: IBindingData(binding)
			, mTextureType(textureType)
		{}

		TextureType mTextureType;
	};

} // namespace erm
