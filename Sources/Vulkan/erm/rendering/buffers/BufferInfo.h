#pragma once

#include <vulkan/vulkan.hpp>

namespace erm {

struct BufferInfo
{
	BufferInfo(
		size_t offset = 0,
		size_t stride = VK_WHOLE_SIZE)
		: mOffset(offset)
		, mStride(stride)
	{}

	size_t mOffset;
	size_t mStride;
};

} // namespace erm