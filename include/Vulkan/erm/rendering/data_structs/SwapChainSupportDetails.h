#pragma once

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {

	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR mCapabilities;
		std::vector<vk::SurfaceFormatKHR> mFormats;
		std::vector<vk::PresentModeKHR> mPresentModes;
	};

} // namespace erm
