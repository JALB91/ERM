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
		extern VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device);
		
		extern VkFormat FindSupportedDepthFormat(
			VkPhysicalDevice physicalDevice,
			const std::vector<VkFormat>& candidates,
			VkImageTiling tiling,
			VkFormatFeatureFlags features
		);
		extern VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);
		
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
		
		extern void CopyBuffer(
			VkCommandPool commandPool,
			VkDevice device,
			VkQueue graphicsQueue,
			VkBuffer srcBuffer,
			VkBuffer dstBuffer,
			VkDeviceSize size
		);
		
		extern void CreateImage(
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			uint32_t width,
			uint32_t height,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory
		);
		
		extern VkImageView CreateImageView(
			VkDevice device,
			VkImage image,
			VkFormat format,
			VkImageAspectFlags aspectFlags
		);
		
	}
	
}
