#include "erm/rendering/buffers/VertexBuffer.h"

namespace erm {

VertexBuffer::VertexBuffer(
	Device& device,
	size_t size)
	: DeviceBuffer(device, size, vk::BufferUsageFlagBits::eVertexBuffer
#ifdef ERM_RAY_TRACING_ENABLED
					   | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR
#endif
	  )
{}

} // namespace erm
