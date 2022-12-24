#pragma once

#include "erm/rendering/data_structs/UniformBufferObject.h"
#include "erm/rendering/enums/StorageBufferType.h"
#include "erm/rendering/enums/FrameBufferType.h"

#include <erm/resources/textures/TextureType.h>

#include <variant>

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
	UboData(uint32_t binding, uint32_t set, UboId uboId, size_t size, uint32_t offset)
		: IBindingData(binding, set)
		, mUboId(uboId)
		, mStride(size)
		, mOffset(offset)
	{}

	UboId mUboId;
	size_t mStride;
	uint32_t mOffset;
};

struct SamplerData : IBindingData
{
	SamplerData(uint32_t binding, uint32_t set, std::variant<FrameBufferType, TextureType> type)
		: IBindingData(binding, set)
		, mType(type)
	{}

	std::variant<FrameBufferType, TextureType> mType;
};

struct StorageBufferData : IBindingData
{
	StorageBufferData(uint32_t binding, uint32_t set, StorageBufferType type, uint32_t offset)
		: IBindingData(binding, set)
		, mType(type)
		, mOffset(offset)
	{}

	StorageBufferType mType;
	uint32_t mOffset;
};

struct StorageImageData : IBindingData
{
	StorageImageData(uint32_t binding, uint32_t set, FrameBufferType frameBufferType)
		: IBindingData(binding, set)
		, mFrameBufferType(frameBufferType)
	{}

	FrameBufferType mFrameBufferType;
};

#ifdef ERM_RAY_TRACING_ENABLED
using AccelerationStructureData = IBindingData;
#endif

} // namespace erm
