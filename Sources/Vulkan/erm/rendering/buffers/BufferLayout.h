#pragma once

#include "erm/rendering/buffers/BufferInfo.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {

	struct BufferLayout
	{
		BufferLayout()
			: mBuffer(nullptr)
		{}

		vk::Buffer mBuffer;
		std::vector<BufferInfo> mInfos;
	};

} // namespace erm