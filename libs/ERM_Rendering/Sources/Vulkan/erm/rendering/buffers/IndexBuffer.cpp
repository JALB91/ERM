#include "erm/rendering/buffers/IndexBuffer.h"

namespace erm {

IndexBuffer::IndexBuffer(Device& device, u64 size, u32 count)
	: DeviceBuffer(device, size, BufferUsage::INDEX_BUFFER
#ifdef ERM_RAY_TRACING_ENABLED
					   | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::STORAGE_BUFFER | BufferUsage::ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY
#endif
				   )
	, mCount(count)
{}

} // namespace erm
