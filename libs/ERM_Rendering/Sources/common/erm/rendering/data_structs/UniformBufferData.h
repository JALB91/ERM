#pragma once

#include "erm/rendering/data_structs/UniformBufferObject.h"
#include "erm/rendering/enums/StorageBufferType.h"
#include "erm/rendering/enums/FrameBufferType.h"

#include <erm/assets/enums/TextureType.h>

#include <erm/math/Types.h>

#include <variant>

namespace erm {

struct IBindingData
{
	IBindingData(u32 binding, u32 set)
		: mBinding(binding)
		, mSet(set)
	{}
	virtual ~IBindingData() = default;

	u32 mBinding;
	u32 mSet;
};

struct UboData : IBindingData
{
	UboData(u32 binding, u32 set, UboId uboId, u64 size, u32 offset)
		: IBindingData(binding, set)
		, mUboId(uboId)
		, mStride(size)
		, mOffset(offset)
	{}

	UboId mUboId;
	u64 mStride;
	u32 mOffset;
};

struct SamplerData : IBindingData
{
	SamplerData(u32 binding, u32 set, std::variant<FrameBufferType, TextureType> type)
		: IBindingData(binding, set)
		, mType(type)
	{}

	std::variant<FrameBufferType, TextureType> mType;
};

struct StorageBufferData : IBindingData
{
	StorageBufferData(u32 binding, u32 set, StorageBufferType type, u32 offset)
		: IBindingData(binding, set)
		, mType(type)
		, mOffset(offset)
	{}

	StorageBufferType mType;
	u32 mOffset;
};

struct StorageImageData : IBindingData
{
	StorageImageData(u32 binding, u32 set, FrameBufferType frameBufferType)
		: IBindingData(binding, set)
		, mFrameBufferType(frameBufferType)
	{}

	FrameBufferType mFrameBufferType;
};

#ifdef ERM_RAY_TRACING_ENABLED
using AccelerationStructureData = IBindingData;
#endif

} // namespace erm
