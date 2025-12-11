#pragma once

#include <erm/math/Types.h>

namespace erm {
template<typename T>
class ObjectsPool;
}

namespace erm {

namespace internal {

struct ObjectHandleData
{
	u32 mIndex = 0;
	u16 mVersion = 0;
	u16 mGeneration = 0;
};

union ObjectHandleBits
{
	ObjectHandleData mData;
	u64 mRaw;
};

}

template<typename T>
struct ObjectHandle
{
public:
	friend class ObjectsPool<T>;
	
public:
	constexpr ObjectHandle() noexcept
	: mBits(internal::ObjectHandleData(0, 0, 0))
	{}
	
	constexpr bool operator==(const ObjectHandle&) const noexcept = default;
	
	constexpr const internal::ObjectHandleData* operator->() const
	{
		return &mBits.mData;
	}
	
	constexpr bool isValid() const
	{
		return mBits.mRaw != 0;
	}
	
private:
	constexpr ObjectHandle(u32 index, u16 version, u16 generation) noexcept
	: mBits(internal::ObjectHandleData(index, generation))
	{}
	
	internal::ObjectHandleBits mBits;
	
};

}
