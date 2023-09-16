#pragma once

#include "erm/rendering/ray_tracing/RTAccelerationStructure.h"

#include <vulkan/vulkan.hpp>

namespace erm {
class Device;
class GPUModel;
} // namespace erm

namespace erm {

struct RTBlasData
{
	vk::AccelerationStructureGeometryKHR mGeometries;
	vk::AccelerationStructureBuildRangeInfoKHR mInfos;
};

class RTBlas : public RTAccelerationStructure
{
public:
	RTBlas(
		Device& device,
		const GPUModel& model);
	~RTBlas();

	inline const RTBlasData& GetBlasData() const { return mBlasData; }

	void UpdateBlasData();
	void GetBuildInfo(vk::AccelerationStructureBuildGeometryInfoKHR& buildInfo) const;

private:
	Device& mDevice;
	const GPUModel& mModel;
	RTBlasData mBlasData;
	bool mNeedsBuild;
};

} // namespace erm
