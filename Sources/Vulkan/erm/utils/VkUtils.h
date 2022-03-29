#pragma once

#include "erm/rendering/enums/BufferUsage.h"
#include "erm/rendering/enums/MemoryProperty.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {
class Device;
struct QueueFamilyIndices;
struct SwapChainSupportDetails;
} // namespace erm

#define ERM_VK_CHECK(OP)                            \
	{                                               \
		const auto result = OP;                     \
		ERM_ASSERT(result == vk::Result::eSuccess); \
	}

namespace erm::VkUtils {

extern QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);
extern SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);
extern bool CheckDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*>& deviceExtensions);
extern bool IsDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface, const std::vector<const char*>& deviceExtensions);
extern vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
extern vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

extern vk::Format FindSupportedDepthFormat(
	vk::PhysicalDevice physicalDevice,
	const std::vector<vk::Format>& candidates,
	vk::ImageTiling tiling,
	vk::FormatFeatureFlags features);
extern vk::Format FindDepthFormat(vk::PhysicalDevice physicalDevice);

extern vk::Extent2D ChooseSwapExtent(
	const vk::SurfaceCapabilitiesKHR& capabilities,
	int fallbackWidth,
	int fallbackHeight);

extern uint32_t FindMemoryType(
	vk::PhysicalDevice physicalDevice,
	uint32_t typeFilter,
	MemoryPropertyFlags mpf);

extern vk::CommandBuffer BeginSingleTimeCommands(Device& device);

extern void EndSingleTimeCommands(
	Device& device,
	vk::CommandBuffer commandBuffer);

extern void CreateBuffer(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	vk::DeviceSize size,
	vk::Buffer& buffer,
	vk::DeviceMemory& bufferMemory,
	BufferUsageFlags buf,
	MemoryPropertyFlags mpf);

extern void CreateBufferUnique(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	vk::DeviceSize size,
	vk::UniqueBuffer& buffer,
	vk::UniqueDeviceMemory& bufferMemory,
	BufferUsageFlags buf,
	MemoryPropertyFlags mpf);

extern void CopyBufferToBuffer(
	Device& device,
	vk::Buffer& srcBuffer,
	vk::Buffer& dstBuffer,
	vk::DeviceSize size);

extern void CreateImage(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	const vk::ImageCreateInfo& createInfo,
	vk::Image& image,
	vk::DeviceMemory& imageMemory,
	MemoryPropertyFlags mpf);

extern void CreateImageUnique(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	const vk::ImageCreateInfo& createInfo,
	vk::UniqueImage& image,
	vk::UniqueDeviceMemory& imageMemory,
	MemoryPropertyFlags mpf);

extern vk::ImageView CreateImageView(
	vk::Device device,
	const vk::ImageViewCreateInfo& createInfo);

extern vk::UniqueImageView CreateImageViewUnique(
	vk::Device device,
	const vk::ImageViewCreateInfo& createInfo);

extern void TransitionImageLayout(
	Device& device,
	vk::Image image,
	vk::ImageLayout oldLayout,
	vk::ImageLayout newLayout,
	uint32_t mipLevels = 1,
	uint32_t layerCount = 1);

extern void CopyBufferToImage(
	Device& device,
	vk::Buffer buffer,
	vk::Image image,
	uint32_t width,
	uint32_t height,
	uint32_t layerCount = 1);

extern void CreateDeviceLocalBuffer(
	vk::Queue queue,
	vk::CommandPool commandPool,
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	vk::DeviceSize bufferSize,
	void* bufferData,
	vk::Buffer& dstBuffer,
	vk::DeviceMemory& dstBufferMemory,
	BufferUsageFlags buf);

template<typename T, typename S>
T ToVulkanValue(S s);

} // namespace erm::VkUtils
