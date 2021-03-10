#pragma once

namespace erm {

	enum class DescriptorType
	{
		COMBINED_IMAGE_SAMPLER,
		UNIFORM_BUFFER,
		STORAGE_IMAGE,
		STORAGE_BUFFER,
#ifdef ERM_RAY_TRACING_ENABLED
		ACCELERATION_STRUCTURE
#endif
	};

} // namespace erm