#pragma once

#include "erm/rendering/data_structs/UniformBufferObject.h"
#include "erm/rendering/enums/StorageBufferType.h"
#include "erm/rendering/enums/TextureType.h"

namespace erm {

	struct IBindingData
	{
		IBindingData(uint32_t binding, uint32_t set)
			: mBinding(binding)
			, mSet(set)
		{}
		virtual ~IBindingData() = default;

		uint32_t mBinding;
		uint32_t mSet;
	};

	struct UboData : IBindingData
	{
		UboData(UboId uboId, size_t size, uint32_t offset, uint32_t binding, uint32_t set)
			: IBindingData(binding, set)
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
		SamplerData(uint32_t binding, uint32_t set, TextureType textureType)
			: IBindingData(binding, set)
			, mTextureType(textureType)
		{}

		TextureType mTextureType;
	};

	struct StorageBufferData : IBindingData
	{
		StorageBufferData(StorageBufferType type, size_t size, uint32_t offset, uint32_t binding, uint32_t set)
			: IBindingData(binding, set)
			, mType(type)
			, mSize(size)
			, mOffset(offset)
		{}

		StorageBufferType mType;
		size_t mSize;
		uint32_t mOffset;
	};

	using StorageImageData = IBindingData;

} // namespace erm
