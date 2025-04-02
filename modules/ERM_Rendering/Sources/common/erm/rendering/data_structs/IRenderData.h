#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"
#include "erm/rendering/data_structs/PipelineConfigs.h"
#include "erm/rendering/data_structs/StorageBufferResources.h"
#include "erm/rendering/data_structs/UniformBufferObject.h"
#include "erm/rendering/enums/StorageBufferType.h"

#include <erm/math/Types.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace erm {

using UbosMap = std::unordered_map<UboId, std::unique_ptr<IUbo>>;
using StorageBuffersMap = std::unordered_map<StorageBufferType, StorageBufferResources>;

struct IRenderData
{
	IRenderData(const PipelineConfigs& pipelineConfigs)
		: mPipelineConfigs(pipelineConfigs)
	{}
	virtual ~IRenderData() = default;

	IRenderData(IRenderData&& other) noexcept
		: mUbos(std::move(other.mUbos))
		, mSbos(std::move(other.mSbos))
		, mPipelineConfigs(std::move(other.mPipelineConfigs))
	{}

	IRenderData& operator=(IRenderData&& other) noexcept
	{
		mUbos = std::move(other.mUbos);
		mSbos = std::move(other.mSbos);
		mPipelineConfigs = std::move(other.mPipelineConfigs);
		return *this;
	}

	IRenderData(const IRenderData&) = delete;
	IRenderData& operator=(const IRenderData&) = delete;

	template<typename T>
	inline T& getUbo()
	{
		if (mUbos.find(T::ID) == mUbos.end())
		{
			mUbos[T::ID] = std::make_unique<T>();
		}

		return *static_cast<T*>(mUbos[T::ID].get());
	}

	inline bool hasUbo(UboId id) const
	{
		return mUbos.find(id) != mUbos.cend();
	}

	inline void addSbo(StorageBufferType type, u32 id, const DeviceBuffer& buffer)
	{
		mSbos[type].addBuffer(id, buffer);
	}

	inline void removeSbos(u32 id)
	{
		for (auto& [type, resources] : mSbos)
		{
			resources.clearBuffer(id);
		}
	}

	inline bool hasSbos(StorageBufferType type) const
	{
		return mSbos.find(type) != mSbos.cend();
	}

	UbosMap mUbos;
	StorageBuffersMap mSbos;
	PipelineConfigs mPipelineConfigs;
};

} // namespace erm
