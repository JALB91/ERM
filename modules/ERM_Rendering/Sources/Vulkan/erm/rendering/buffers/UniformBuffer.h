#pragma once

#include "erm/rendering/buffers/DeviceBuffer.h"
#include "erm/rendering/buffers/HostBuffer.h"
#include "erm/rendering/buffers/IBuffer.h"

#include <erm/math/Types.h>

#include <type_traits>

namespace erm {

template<
	typename BufferType,
	typename Enable = std::enable_if_t<std::is_base_of_v<IBuffer, BufferType>>>
class UniformBuffer : public BufferType
{
public:
	UniformBuffer(Device& device, u64 size)
		: BufferType(
			  device,
			  size,
			  BufferUsage::UNIFORM_BUFFER
#ifdef ERM_RAY_TRACING_ENABLED
				  | BufferUsage::STORAGE_BUFFER
#endif
		  )
	{}
};

} // namespace erm
