#pragma once

#include "erm/rendering/data_structs/QueueFamilyIndices.h"
#include "erm/rendering/data_structs/SwapChainSupportDetails.h"

#include <vulkan/vulkan.hpp>

#include <vector>

#define VK_CHECK(OP)                            \
	{                                           \
		const auto result = OP;                 \
		ASSERT(result == vk::Result::eSuccess); \
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
		vk::MemoryPropertyFlags properties);

	extern vk::CommandBuffer BeginSingleTimeCommands(
		vk::CommandPool commandPool,
		vk::Device device);

	extern void EndSingleTimeCommands(
		vk::Queue graphicsQueue,
		vk::CommandPool commandPool,
		vk::Device device,
		vk::CommandBuffer commandBuffer);

	extern void CreateBuffer(
		vk::PhysicalDevice physicalDevice,
		vk::Device device,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Buffer& buffer,
		vk::DeviceMemory& bufferMemory);

	extern void CreateBufferUnique(
		vk::PhysicalDevice physicalDevice,
		vk::Device device,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::UniqueBuffer& buffer,
		vk::UniqueDeviceMemory& bufferMemory);

	extern void CopyBufferToBuffer(
		vk::CommandPool commandPool,
		vk::Device device,
		vk::Queue graphicsQueue,
		vk::Buffer& srcBuffer,
		vk::Buffer& dstBuffer,
		vk::DeviceSize size);

	extern void CreateImage(
		vk::PhysicalDevice physicalDevice,
		vk::Device device,
		uint32_t width,
		uint32_t height,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Image& image,
		vk::DeviceMemory& imageMemory);

	extern void CreateImageUnique(
		vk::PhysicalDevice physicalDevice,
		vk::Device device,
		uint32_t width,
		uint32_t height,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::UniqueImage& image,
		vk::UniqueDeviceMemory& imageMemory);

	extern vk::ImageView CreateImageView(
		vk::Device device,
		vk::Image image,
		vk::Format format,
		vk::ImageAspectFlags aspectFlags);

	extern vk::UniqueImageView CreateImageViewUnique(
		vk::Device device,
		vk::Image image,
		vk::Format format,
		vk::ImageAspectFlags aspectFlags);

	extern void TransitionImageLayout(
		vk::Queue graphicsQueue,
		vk::CommandPool commandPool,
		vk::Device device,
		vk::Image image,
		vk::Format /*format*/,
		vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout);

	extern void CopyBufferToImage(
		vk::Queue graphicsQueue,
		vk::CommandPool commandPool,
		vk::Device device,
		vk::Buffer& buffer,
		vk::Image image,
		uint32_t width,
		uint32_t height);

	extern void CreateDeviceLocalBuffer(
		vk::Queue queue,
		vk::CommandPool commandPool,
		vk::PhysicalDevice physicalDevice,
		vk::Device device,
		vk::BufferUsageFlags bufferUsage,
		vk::DeviceSize bufferSize,
		void* bufferData,
		vk::Buffer& dstBuffer,
		vk::DeviceMemory& dstBufferMemory);

	template<typename T, typename S>
	T ToVulkanValue(S s);

} // namespace erm::VkUtils
