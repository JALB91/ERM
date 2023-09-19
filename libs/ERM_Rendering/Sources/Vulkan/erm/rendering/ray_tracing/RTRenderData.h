#pragma once

#include "erm/rendering/ray_tracing/RTInstanceData.h"
#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/math/Types.h>

#include <unordered_map>

namespace erm {

using InstancesMap = std::unordered_map<u32, RTInstanceData>;

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
		{
			return *this;
		}

		ERM_ASSERT(&mDevice == &other.mDevice);
		IRenderData::operator=(std::move(other));
		mInstancesMap = std::move(other.mInstancesMap);
		mForceUpdate = other.mForceUpdate;

		return *this;
	}

	inline void addOrUpdateInstance(RTBlas* blas, const mat4& transform, u32 index)
	{
		if (auto it = mInstancesMap.find(index); it != mInstancesMap.cend())
		{
			it->second.update(blas, transform);
		}
		else
		{
			mInstancesMap.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(index),
				std::forward_as_tuple(blas, transform));
		}
	}

	inline void clearDataForIndex(u32 id)
	{
		ERM_VK_CHECK(mDevice.getGraphicsQueue().waitIdle());

		if (const auto it = mInstancesMap.find(id); it != mInstancesMap.cend())
		{
			mInstancesMap.erase(it);
		}

		removeSbos(id);
	}

	inline bool hasInstanceWithId(u32 id) const
	{
		return mInstancesMap.find(id) != mInstancesMap.cend();
	}

	Device& mDevice;
	InstancesMap mInstancesMap;
	bool mForceUpdate;
};

} // namespace erm
