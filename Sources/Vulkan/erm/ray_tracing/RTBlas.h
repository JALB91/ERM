#pragma once

#include "erm/ray_tracing/RTAccelerationStructure.h"

#include <vulkan/vulkan.hpp>

#include <memory>

namespace erm {
	class Device;
	class Model;
} // namespace erm

namespace erm {

	struct RTBlasData
	{
		std::vector<vk::AccelerationStructureGeometryKHR> mGeometries;
		std::vector<vk::AccelerationStructureBuildRangeInfoKHR> mInfos;
	};

	class RTBlas : public RTAccelerationStructure
	{
	public:
		RTBlas(
			Device& device,
			const Model& model);
		~RTBlas();

		void UpdateBlasData();
		void GetBuildInfo(vk::AccelerationStructureBuildGeometryInfoKHR& buildInfo) const;

		inline uint32_t GetId() const { return mID; }
		inline const RTBlasData& GetBlasData() const { return mBlasData; }

	private:
		static uint32_t sCurrentId;

		Device& mDevice;
		const Model& mModel;
		RTBlasData mBlasData;
		uint32_t mID;
	};

} // namespace erm
