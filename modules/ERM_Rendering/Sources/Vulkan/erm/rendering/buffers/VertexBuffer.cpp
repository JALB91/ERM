#include "erm/rendering/buffers/VertexBuffer.h"

namespace erm {

VertexBuffer::VertexBuffer(
	Device& device,
	u64 size)
	: DeviceBuffer(device, size, BufferUsage::VERTEX_BUFFER
#ifdef ERM_RAY_TRACING_ENABLED
					   | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::STORAGE_BUFFER | BufferUsage::ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY
#endif
	  )
{}

} // namespace erm
