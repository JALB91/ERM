#pragma once

#include "erm/utils/ObjectHandle.h"
#include "erm/utils/IObjectsPool.h"

#include <erm/math/Types.h>

#include <refl.hpp>

#include <memory>
#include <vector>

namespace erm {

template<typename T>
class ObjectsPool : public IObjectsPool
{
private:
	struct ObjectEntry
	{
		std::unique_ptr<T> mObject = nullptr;
		u32 mGeneration = 0;
	};
	
public:
	ObjectHandle<T> load(std::unique_ptr<T> asset)
	{
		u32 index;
		u32 generation;

		if (!mFreeList.empty())
		{
			index = mFreeList.back();
			mFreeList.pop_back();

			auto& entry = mEntries[index];
			entry.mObject = std::move(asset);
		}
		else
		{
			index = static_cast<u32>(mEntries.size());
			generation = 1;
			mEntries.emplace_back(
				std::move(asset),
				generation
			);
		}

		return ObjectHandle<T>{
			index,
			generation
		};
	}

	T* get(ObjectHandle<T> handle)
	{
		if (handle->mIndex >= mEntries.size())
		{
			return nullptr;
		}

		auto& entry = mEntries[handle->mIndex];
		if (entry.mGeneration != handle->mGeneration)
		{
			return nullptr;
		}

		return entry.mObject.get();
	}

	bool unload(ObjectHandle<T> handle)
	{
		if (handle->mIndex >= mEntries.size())
		{
			return false;
		}

		auto& entry = mEntries[handle->mIndex];
		if (entry.mGeneration != handle->mGeneration)
		{
			return false;
		}

		entry.mObject.reset();
		++entry.mGeneration;
		
		mFreeList.emplace_back(handle->mIndex);
		
		return true;
	}

	void clear() override
	{
		mEntries.clear();
		mFreeList.clear();
	}
	
private:
	std::vector<ObjectEntry> mEntries;
	std::vector<u32> mFreeList;
	
};

}

REFL_TEMPLATE(
	(typename T),
	(erm::ObjectsPool<T>))
REFL_END
