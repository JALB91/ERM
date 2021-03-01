#pragma once

#include "erm/ray_tracing/RTInstanceData.h"
#include "erm/ray_tracing/RTRenderConfigs.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/IRenderData.h"

#include <unordered_map>

namespace erm {

	struct RTRenderData : public IRenderData
	{
		RTRenderData(Device& device, const RTRenderConfigs& renderConfigs = {})
			: mDevice(device)
			, mRenderConfigs(renderConfigs)
			, mForceUpdate(true)
		{}

		RTRenderData(RTRenderData&& other)
			: IRenderData(std::move(other))
			, mDevice(other.mDevice)
			, mRenderConfigs(std::move(other.mRenderConfigs))
			, mInstancesMap(std::move(other.mInstancesMap))
			, mForceUpdate(other.mForceUpdate)
		{}

		RTRenderData& operator=(RTRenderData&& other)
		{
			if (this == &other)
				return *this;

			ASSERT(&mDevice == &other.mDevice);
			IRenderData::operator=(std::move(other));
			mRenderConfigs = std::move(other.mRenderConfigs);
			mInstancesMap = std::move(other.mInstancesMap);
			mForceUpdate = other.mForceUpdate;

			return *this;
		}

		inline RTInstanceData& AddOrUpdateInstance(RTBlas* blas, const math::mat4& transform, uint32_t index)
		{
			if (auto it = mInstancesMap.find(index); it != mInstancesMap.cend())
			{
				it->second.Update(blas, transform);
			}

			auto it = mInstancesMap.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(index),
				std::forward_as_tuple(blas, transform));
			return it.first->second;
		}

		inline void ClearDataForIndex(uint32_t id)
		{
			mDevice.GetGraphicsQueue().waitIdle();

			if (const auto it = mInstancesMap.find(id); it != mInstancesMap.cend())
				mInstancesMap.erase(it);

			RemoveSbos(id);
		}

		inline bool HasInstanceWithId(uint32_t id) const
		{
			return mInstancesMap.find(id) != mInstancesMap.cend();
		}

		Device& mDevice;
		RTRenderConfigs mRenderConfigs;
		std::unordered_map<uint32_t, RTInstanceData> mInstancesMap;
		bool mForceUpdate;
	};

} // namespace erm
