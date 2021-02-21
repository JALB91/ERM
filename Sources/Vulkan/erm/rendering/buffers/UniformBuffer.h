#pragma once

#include "erm/rendering/buffers/IBuffer.h"
#include "erm/rendering/buffers/DeviceBuffer.h"
#include "erm/rendering/buffers/HostBuffer.h"

#include <type_traits>

namespace erm {

	template<
		typename BufferType,
		typename Enable = std::enable_if_t<std::is_base_of_v<IBuffer, BufferType>>
	>
	class UniformBuffer : public BufferType
	{
	public:
		UniformBuffer(Device& device, size_t size)
			: BufferType(
				device,
				size,
				vk::BufferUsageFlagBits::eUniformBuffer)
		{}
	};

} // namespace erm
