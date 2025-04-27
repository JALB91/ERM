#pragma once

#include "erm/rendering/enums/BufferUsage.h"
#include "erm/rendering/enums/MemoryProperty.h"

#include <erm/log/Assert.h>

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {
class Device;
struct QueueFamilyIndices;
struct SwapChainSupportDetails;
} // namespace erm

#ifdef ERM_DEBUG
#define ERM_VK_CHECK(OP)                            \
	{                                               \
		const auto _result = OP;                     \
		ERM_ASSERT(_result == vk::Result::eSuccess); \
	}

#define ERM_VK_CHECK_AND_RETURN(OP)                        \
	{                                                      \
		auto _result = OP;                            \
		ERM_ASSERT(_result.result == vk::Result::eSuccess); \
		return std::forward<decltype(_result.value)>(_result.value); \
	}

#define ERM_VK_CHECK_AND_ASSIGN(DEST, OP)                  \
	{                                                      \
		auto _result = OP;                            \
		ERM_ASSERT(_result.result == vk::Result::eSuccess); \
		DEST = std::forward<decltype(_result.value)>(_result.value);                               \
	}
#else
#define ERM_VK_CHECK(OP)        \
	{                           \
		const auto _result = OP; \
		ERM_UNUSED(_result);     \
	}

#define ERM_VK_CHECK_AND_RETURN(OP) return OP.value;

#define ERM_VK_CHECK_AND_ASSIGN(DEST, OP) DEST = OP.value;
#endif

namespace erm::VkUtils {

extern QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);
extern SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);
extern bool checkDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*>& deviceExtensions);
extern bool isDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface, const std::vector<const char*>& deviceExtensions);
extern vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
extern vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

extern vk::Format findSupportedDepthFormat(
	vk::PhysicalDevice physicalDevice,
	const std::vector<vk::Format>& candidates,
	vk::ImageTiling tiling,
	vk::FormatFeatureFlags features);
extern vk::Format findDepthFormat(vk::PhysicalDevice physicalDevice);

extern vk::Extent2D chooseSwapExtent(
	const vk::SurfaceCapabilitiesKHR& capabilities,
	u32 fallbackWidth,
	u32 fallbackHeight);

extern u32 findMemoryType(
	vk::PhysicalDevice physicalDevice,
	u32 typeFilter,
	MemoryPropertyFlags mpf);

extern vk::CommandBuffer beginSingleTimeCommands(Device& device);

extern void endSingleTimeCommands(
	Device& device,
	vk::CommandBuffer commandBuffer);

extern void createBuffer(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	vk::DeviceSize size,
	vk::Buffer& buffer,
	vk::DeviceMemory& bufferMemory,
	BufferUsageFlags buf,
	MemoryPropertyFlags mpf);

extern void createBufferUnique(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	vk::DeviceSize size,
	vk::UniqueBuffer& buffer,
	vk::UniqueDeviceMemory& bufferMemory,
	BufferUsageFlags buf,
	MemoryPropertyFlags mpf);

extern void copyBufferToBuffer(
	Device& device,
	vk::Buffer& srcBuffer,
	vk::Buffer& dstBuffer,
	vk::DeviceSize size);

extern void createImage(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	const vk::ImageCreateInfo& createInfo,
	vk::Image& image,
	vk::DeviceMemory& imageMemory,
	MemoryPropertyFlags mpf);

extern void createImageUnique(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	const vk::ImageCreateInfo& createInfo,
	vk::UniqueImage& image,
	vk::UniqueDeviceMemory& imageMemory,
	MemoryPropertyFlags mpf);

extern vk::ImageView createImageView(
	vk::Device device,
	const vk::ImageViewCreateInfo& createInfo);

extern vk::UniqueImageView createImageViewUnique(
	vk::Device device,
	const vk::ImageViewCreateInfo& createInfo);

extern void transitionImageLayout(
	Device& device,
	vk::Image image,
	vk::ImageLayout oldLayout,
	vk::ImageLayout newLayout,
	u32 mipLevels = 1,
	u32 layerCount = 1);

extern void copyBufferToImage(
	Device& device,
	vk::Buffer buffer,
	vk::Image image,
	u32 width,
	u32 height,
	u32 layerCount = 1);

template<typename T, typename S>
T toVulkanValue(S s);

} // namespace erm::VkUtils
