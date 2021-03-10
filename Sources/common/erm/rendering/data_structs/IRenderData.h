#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"
#include "erm/rendering/data_structs/StorageBufferResources.h"
#include "erm/rendering/data_structs/UniformBufferObject.h"
#include "erm/rendering/enums/StorageBufferType.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace erm {

	using UbosMap = std::unordered_map<UboId, std::unique_ptr<IUbo>>;
	using StorageBuffersMap = std::unordered_map<StorageBufferType, StorageBufferResources>;

	struct IRenderData
	{
		IRenderData() = default;
		virtual ~IRenderData() = default;

		IRenderData(IRenderData&& other)
			: mUbos(std::move(other.mUbos))
			, mSbos(std::move(other.mSbos))
		{}

		IRenderData& operator=(IRenderData&& other)
		{
			mUbos = std::move(other.mUbos);
			mSbos = std::move(other.mSbos);
			return *this;
		}

		template<typename T>
		inline void SetUbo(T ubo)
		{
			if (mUbos.find(T::ID) == mUbos.end())
				mUbos[T::ID] = std::make_unique<T>();

			*static_cast<T*>(mUbos[T::ID].get()) = ubo;
		}

		inline bool HasUbo(UboId id) const
		{
			return mUbos.find(id) != mUbos.cend();
		}

		inline void AddSbo(StorageBufferType type, uint32_t id, const DeviceBuffer& buffer)
		{
			mSbos[type].AddBuffer(id, buffer);
		}

		inline void RemoveSbos(uint32_t id)
		{
			for (auto& [type, resources] : mSbos)
				resources.ClearBuffer(id);
		}

		inline bool HasSbos(StorageBufferType type) const
		{
			return mSbos.find(type) != mSbos.cend();
		}

		UbosMap mUbos;
		StorageBuffersMap mSbos;
	};

} // namespace erm