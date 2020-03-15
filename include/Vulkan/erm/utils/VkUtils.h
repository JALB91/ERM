#pragma once

#include "erm/rendering/data_structs/QueueFamilyIndices.h"
#include "erm/rendering/data_structs/SwapChainSupportDetails.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace erm {
	
	namespace Utils {
		
		extern QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		extern SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		
		extern bool CheckDeviceExtensionSupport(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions);
		
		extern bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions);
		
		extern VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		
		extern VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		
		extern VkExtent2D ChooseSwapExtent(
			const VkSurfaceCapabilitiesKHR& capabilities,
			int fallbackWidth,
			int fallbackHeight
		);
		
		extern uint32_t FindMemoryType(
			VkPhysicalDevice physicalDevice,
			uint32_t typeFilter,
			VkMemoryPropertyFlags properties
		);
		
		extern void CreateBuffer(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& bufferMemory
		);
		
	}
	
}
