#pragma once

#include <vulkan/vulkan.hpp>

namespace erm {

class IExtCommandBufferUpdater
{
public:
	virtual ~IExtCommandBufferUpdater() = default;
	
	virtual void updateCommandBuffer(vk::CommandBuffer& cmd) = 0;
};

}

