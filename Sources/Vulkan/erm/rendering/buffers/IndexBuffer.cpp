#include "erm/rendering/buffers/IndexBuffer.h"

namespace erm {

	IndexBuffer::IndexBuffer(Device& device, size_t size, uint32_t count)
		: DeviceBuffer(device, size, vk::BufferUsageFlagBits::eIndexBuffer
#ifdef ERM_RAY_TRACING_ENABLED
						   | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR
#endif
					   )
		, mCount(count)
	{}

} // namespace erm
