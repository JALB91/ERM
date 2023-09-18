#pragma once

#include "erm/rendering/ray_tracing/RTInstanceData.h"
#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/utils/VkUtils.h"

#include <unordered_map>

namespace erm {

using InstancesMap = std::unordered_map<uint32_t, RTInstanceData>;

struct RTRenderData : public IRenderData
{
	RTRenderData(Device& device, const PipelineConfigs& pipelineConfigs = PipelineConfigs::DEFAULT)
		: IRenderData(pipelineConfigs)
		, mDevice(device)
		, mForceUpdate(true)
	{}

	RTRenderData(RTRenderData&& other) noexcept
		: IRenderData(std::move(other))
		, mDevice(other.mDevice)
		, mInstancesMap(std::move(other.mInstancesMap))
		, mForceUpdate(other.mForceUpdate)
	{}

	RTRenderData& operator=(RTRenderData&& other) noexcept
	{
		if (this == &other)
			return *this;

		ERM_ASSERT(&mDevice == &other.mDevice);
		IRenderData::operator=(std::move(other));
		mInstancesMap = std::move(other.mInstancesMap);
		mForceUpdate = other.mForceUpdate;

		return *this;
	}

	inline void AddOrUpdateInstance(RTBlas* blas, const math::mat4& transform, uint32_t index)
	{
		if (auto it = mInstancesMap.find(index); it != mInstancesMap.cend())
		{
			it->second.Update(blas, transform);
		}
		else
		{
			mInstancesMap.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(index),
				std::forward_as_tuple(blas, transform));
		}
	}

	inline void ClearDataForIndex(uint32_t id)
	{
		ERM_VK_CHECK(mDevice.GetGraphicsQueue().waitIdle());

		if (const auto it = mInstancesMap.find(id); it != mInstancesMap.cend())
			mInstancesMap.erase(it);

		RemoveSbos(id);
	}

	inline bool HasInstanceWithId(uint32_t id) const
	{
		return mInstancesMap.find(id) != mInstancesMap.cend();
	}

	Device& mDevice;
	InstancesMap mInstancesMap;
	bool mForceUpdate;
};

} // namespace erm
