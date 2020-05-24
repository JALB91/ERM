#include "erm/utils/VkUtils.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <set>
#include <stdexcept>

namespace erm::VkUtils {

	QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface)
	{
		QueueFamilyIndices indices;

		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); ++i)
		{
			if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
			{
				indices.mGraphicsFamily = i;
			}

			if (device.getSurfaceSupportKHR(i, surface))
			{
				indices.mPresentFamily = i;
			}
		}

		return indices;
	}

	SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
	{
		SwapChainSupportDetails details;

		details.mCapabilities = device.getSurfaceCapabilitiesKHR(surface);
		details.mFormats = device.getSurfaceFormatsKHR(surface);
		details.mPresentModes = device.getSurfacePresentModesKHR(surface);

		return details;
	}

	bool CheckDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*>& deviceExtensions)
	{
		std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const vk::ExtensionProperties& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	bool IsDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface, const std::vector<const char*>& deviceExtensions)
	{
		vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
		const bool isSuitable = FindQueueFamilies(device, surface).IsComplete() && CheckDeviceExtensionSupport(device, deviceExtensions);

		bool swapChainAdequate = false;
		if (isSuitable)
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
			swapChainAdequate = !swapChainSupport.mFormats.empty() && !swapChainSupport.mPresentModes.empty();
		}

		std::cout << "Checking compatibility of: " << deviceProperties.deviceName << " -> " << std::boolalpha << (isSuitable && swapChainAdequate) << std::endl;

		vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();

		return isSuitable && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		for (const vk::SurfaceFormatKHR& availableFormat : availableFormats)
		{
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			{
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
	{
		for (const vk::PresentModeKHR& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == vk::PresentModeKHR::eMailbox)
			{
				return availablePresentMode;
			}
		}
		return vk::PresentModeKHR::eFifo;
	}

	vk::DescriptorSetLayout CreateDescriptorSetLayout(vk::Device device)
	{
		vk::DescriptorSetLayoutBinding uboLayoutBinding {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		vk::DescriptorSetLayoutBinding samplerLayoutBinding {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

		std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

		vk::DescriptorSetLayoutCreateInfo layoutInfo {};
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		return device.createDescriptorSetLayout(layoutInfo);
	}

	vk::Format FindSupportedDepthFormat(
		vk::PhysicalDevice physicalDevice,
		const std::vector<vk::Format>& candidates,
		vk::ImageTiling tiling,
		vk::FormatFeatureFlags features)
	{
		for (vk::Format format : candidates)
		{
			vk::FormatProperties props = physicalDevice.getFormatProperties(format);

			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}
		throw std::runtime_error("Failed to find supported format");
	}

	vk::Format FindDepthFormat(vk::PhysicalDevice physicalDevice)
	{
		return FindSupportedDepthFormat(
			physicalDevice,
			{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	}

	vk::CommandBuffer BeginSingleTimeCommands(
		vk::CommandPool commandPool,
		vk::Device device)
	{
		vk::CommandBufferAllocateInfo allocInfo {};
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		vk::CommandBuffer commandBuffer;
		device.allocateCommandBuffers(&allocInfo, &commandBuffer);

		vk::CommandBufferBeginInfo beginInfo {};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		commandBuffer.begin(beginInfo);

		return commandBuffer;
	}

	void EndSingleTimeCommands(
		vk::Queue graphicsQueue,
		vk::CommandPool commandPool,
		vk::Device device,
		vk::CommandBuffer commandBuffer)
	{
		commandBuffer.end();

		vk::SubmitInfo submitInfo {};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		graphicsQueue.submit(1, &submitInfo, nullptr);
		vkQueueWaitIdle(graphicsQueue);

		device.freeCommandBuffers(commandPool, 1, &commandBuffer);
	}

	vk::Extent2D ChooseSwapExtent(
		const vk::SurfaceCapabilitiesKHR& capabilities,
		int fallbackWidth,
		int fallbackHeight)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			vk::Extent2D actualExtent = {static_cast<uint32_t>(fallbackWidth), static_cast<uint32_t>(fallbackHeight)};

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	uint32_t FindMemoryType(
		vk::PhysicalDevice physicalDevice,
		uint32_t typeFilter,
		vk::MemoryPropertyFlags properties)
	{
		vk::PhysicalDeviceMemoryProperties memProperties;
		physicalDevice.getMemoryProperties(&memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type");
	}

	void CreateBuffer(
		vk::PhysicalDevice physicalDevice,
		vk::Device device,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Buffer& buffer,
		vk::DeviceMemory& bufferMemory)
	{
		vk::BufferCreateInfo bufferInfo = {};
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		buffer = device.createBuffer(bufferInfo);

		vk::MemoryRequirements memRequirements;
		device.getBufferMemoryRequirements(buffer, &memRequirements);

		vk::MemoryAllocateInfo allocInfo = {};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		bufferMemory = device.allocateMemory(allocInfo);
		device.bindBufferMemory(buffer, bufferMemory, 0);
	}

	void CopyBufferToBuffer(
		vk::CommandPool commandPool,
		vk::Device device,
		vk::Queue graphicsQueue,
		vk::Buffer& srcBuffer,
		vk::Buffer& dstBuffer,
		vk::DeviceSize size)
	{
		vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(commandPool, device);

		vk::BufferCopy copyRegion {};
		copyRegion.size = size;
		commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

		EndSingleTimeCommands(graphicsQueue, commandPool, device, commandBuffer);
	}

	void CreateImage(
		vk::PhysicalDevice physicalDevice,
		vk::Device device,
		uint32_t width,
		uint32_t height,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::Image& image,
		vk::DeviceMemory& imageMemory)
	{
		vk::ImageCreateInfo imageInfo {};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = usage;
		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;

		image = device.createImage(imageInfo);

		vk::MemoryRequirements memRequirements;
		device.getImageMemoryRequirements(image, &memRequirements);

		vk::MemoryAllocateInfo allocInfo {};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		imageMemory = device.allocateMemory(allocInfo);
		device.bindImageMemory(image, imageMemory, 0);
	}

	vk::ImageView CreateImageView(
		vk::Device device,
		vk::Image image,
		vk::Format format,
		vk::ImageAspectFlags aspectFlags)
	{
		vk::ImageViewCreateInfo viewInfo {};
		viewInfo.image = image;
		viewInfo.viewType = vk::ImageViewType::e2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		vk::ImageView imageView = device.createImageView(viewInfo);

		return imageView;
	}

	void TransitionImageLayout(
		vk::Queue graphicsQueue,
		vk::CommandPool commandPool,
		vk::Device device,
		vk::Image image,
		vk::Format /*format*/,
		vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout)
	{
		vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(commandPool, device);

		vk::ImageMemoryBarrier barrier {};
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else
		{
			throw std::invalid_argument("Unsupported layout transition");
		}

		commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, 0, nullptr, 1, &barrier);

		EndSingleTimeCommands(graphicsQueue, commandPool, device, commandBuffer);
	}

	void CopyBufferToImage(
		vk::Queue graphicsQueue,
		vk::CommandPool commandPool,
		vk::Device device,
		vk::Buffer& buffer,
		vk::Image image,
		uint32_t width,
		uint32_t height)
	{
		vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(commandPool, device);

		vk::BufferImageCopy region {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = vk::Offset3D(0, 0, 0);
		region.imageExtent = vk::Extent3D(width, height, 1);

		commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

		EndSingleTimeCommands(graphicsQueue, commandPool, device, commandBuffer);
	}

	void CreateDeviceLocalBuffer(
		vk::Queue queue,
		vk::CommandPool commandPool,
		vk::PhysicalDevice physicalDevice,
		vk::Device device,
		vk::BufferUsageFlags bufferUsage,
		vk::DeviceSize bufferSize,
		void* bufferData,
		vk::Buffer& dstBuffer,
		vk::DeviceMemory& dstBufferMemory)
	{
		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		CreateBuffer(
			physicalDevice,
			device,
			bufferSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
		memcpy(data, bufferData, (size_t)bufferSize);
		device.unmapMemory(stagingBufferMemory);

		CreateBuffer(
			physicalDevice,
			device,
			bufferSize,
			vk::BufferUsageFlagBits::eTransferDst | bufferUsage,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			dstBuffer,
			dstBufferMemory);

		CopyBufferToBuffer(
			commandPool,
			device,
			queue,
			stagingBuffer,
			dstBuffer,
			bufferSize);

		device.destroyBuffer(stagingBuffer);
		device.freeMemory(stagingBufferMemory);
	}

} // namespace erm::VkUtils
