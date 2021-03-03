#pragma once

#include "erm/rendering/buffers/BufferInfo.h"

#include <vulkan/vulkan.hpp>

namespace erm {

	struct BufferHandle
	{
		BufferHandle(
			vk::Buffer buffer,
			const BufferInfo& info)
			: mBuffer(buffer)
			, mInfo(info)
		{}

		vk::Buffer mBuffer;
		BufferInfo mInfo;
	};

} // namespace erm