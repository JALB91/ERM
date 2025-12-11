#pragma once

#include "erm/utils/ObjectHandle.h"
#include "erm/utils/IObjectsPool.h"

#include <erm/math/Types.h>

#include <refl.hpp>

#include <expected>
#include <memory>
#include <vector>

namespace erm {

template<typename T>
class ObjectsPool : public IObjectsPool
{
public:
	enum class GetErrorType
	{
		INVALID_INDEX,
		INVALID_GENERATION,
		INVALID_VERSION,
		NULL_OBJECT
	};

private:
	struct ObjectEntry
	{
		std::unique_ptr<T> mObject = nullptr;
		u16 mVersion = 0;
		u16 mGeneration = 0;
	};
	
public:
	ObjectHandle<T> load(std::unique_ptr<T> asset)
	{
		u32 index;
		u16 generation;

		if (!mFreeList.empty())
		{
			index = mFreeList.back();
			mFreeList.pop_back();

			auto& entry = mEntries[index];
			entry.mObject = std::move(asset);

			generation = entry.mGeneration;
		}
		else
		{
			index = static_cast<u32>(mEntries.size());
			generation = 1;
			mEntries.emplace_back(
				std::move(asset),
				1,
				generation
			);
		}

		return ObjectHandle<T>(
			index,
			1,
			generation
		);
	}

	std::expected<T*, GetErrorType> get(ObjectHandle<T> handle)
	{
		if (handle->mIndex >= mEntries.size())
		{
			return std::unexpected(GetErrorType::INVALID_INDEX);
		}

		auto& entry = mEntries[handle->mIndex];
		if (entry.mGeneration != handle->mGeneration)
		{
			return std::unexpected(GetErrorType::INVALID_GENERATION);
		}

		if (entry.mVersion != handle->mVersion)
		{
			return std::unexpected(GetErrorType::INVALID_VERSION);
		}

		if (!entry.mObject)
		{
			return std::unexpected(GetErrorType::NULL_OBJECT);
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
