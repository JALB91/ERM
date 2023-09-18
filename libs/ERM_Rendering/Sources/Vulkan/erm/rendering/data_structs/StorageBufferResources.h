#pragma once

#include "erm/rendering/buffers/BufferInfo.h"

#include <vulkan/vulkan.hpp>

#include <unordered_map>

namespace erm {

class StorageBufferResources
{
public:
	StorageBufferResources()
		: mMaxId(0)
	{}

	inline uint32_t GetMaxBufferId() const { return mMaxId; }

	inline void AddBuffer(uint32_t id, const IBuffer& buffer)
	{
		mMaxId = std::max(mMaxId, id);
		mStorageBuffers[id] = buffer.GetBuffer();
	}

	inline void ClearBuffer(uint32_t id)
	{
		if (const auto it = mStorageBuffers.find(id); it != mStorageBuffers.end())
		{
			mStorageBuffers.erase(it);

			if (id == mMaxId)
			{
				mMaxId = 0;
				std::for_each(mStorageBuffers.begin(), mStorageBuffers.end(), [this](const auto& pair) {
					mMaxId = std::max(pair.first, mMaxId);
				});
			}
		}
	}

	inline vk::Buffer GetBuffer(uint32_t id) const
	{
		if (const auto& it = mStorageBuffers.find(id); it != mStorageBuffers.cend())
			return it->second;
		return nullptr;
	}

private:
	std::unordered_map<uint32_t, vk::Buffer> mStorageBuffers;
	uint32_t mMaxId;
};

} // namespace erm