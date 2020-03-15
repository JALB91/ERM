#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace erm {
	
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};
	
}
