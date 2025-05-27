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
	u32 mGeneration = 0;
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
	ObjectHandle() noexcept
	: mBits(internal::ObjectHandleData(0, 0))
	{}
	
	bool operator==(const ObjectHandle&) const noexcept = default;
	
	const internal::ObjectHandleData* operator->() const
	{
		return &mBits.mData;
	}
	
	bool isValid() const
	{
		return mBits.mRaw != 0;
	}
	
private:
	ObjectHandle(u32 index, u32 generation) noexcept
	: mBits(internal::ObjectHandleData(index, generation))
	{}
	
	internal::ObjectHandleBits mBits;
	
};

}
