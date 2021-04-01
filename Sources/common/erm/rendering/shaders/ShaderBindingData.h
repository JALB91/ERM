#pragma once

#include "erm/rendering/data_structs/UniformBufferData.h"

#include <vector>

namespace erm {

struct ShaderBindingData
{
	std::vector<UboData> mUbosData;
	std::vector<SamplerData> mSamplersData;
	std::vector<StorageImageData> mStorageImagesData;
	std::vector<StorageBufferData> mStorageBuffersData;
#ifdef ERM_RAY_TRACING_ENABLED
	std::vector<AccelerationStructureData> mASData;
#endif
};

} // namespace erm