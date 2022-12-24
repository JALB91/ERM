#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"
#include "erm/rendering/data_structs/PipelineConfigs.h"
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
	inline T& GetUbo()
	{
		if (mUbos.find(T::ID) == mUbos.end())
			mUbos[T::ID] = std::make_unique<T>();

		return *static_cast<T*>(mUbos[T::ID].get());
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
	PipelineConfigs mPipelineConfigs;
};

} // namespace erm
