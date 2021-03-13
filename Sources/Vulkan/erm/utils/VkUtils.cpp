#include "erm/utils/VkUtils.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/enums/AttachmentLoadOp.h"
#include "erm/rendering/enums/AttachmentStoreOp.h"
#include "erm/rendering/enums/CullMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/DescriptorType.h"
#include "erm/rendering/enums/DrawMode.h"
#include "erm/rendering/enums/FrontFace.h"
#include "erm/rendering/enums/ImageLayout.h"
#include "erm/rendering/enums/PolygonMode.h"
#include "erm/rendering/enums/ShaderType.h"

#include "erm/utils/Utils.h"

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

		return isSuitable && swapChainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.fillModeNonSolid;
	}

	vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		for (const vk::SurfaceFormatKHR& availableFormat : availableFormats)
		{
#ifdef ERM_RAY_TRACING_ENABLED
			if (availableFormat.format == vk::Format::eR8G8B8A8Unorm)
#else
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
#endif
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

	vk::CommandBuffer BeginSingleTimeCommands(Device& device)
	{
		vk::CommandBufferAllocateInfo allocInfo {};
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandPool = device.GetCommandPool();
		allocInfo.commandBufferCount = 1;

		vk::CommandBuffer commandBuffer;
		VK_CHECK(device->allocateCommandBuffers(&allocInfo, &commandBuffer));

		vk::CommandBufferBeginInfo beginInfo {};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		commandBuffer.begin(beginInfo);

		return commandBuffer;
	}

	void EndSingleTimeCommands(
		Device& device,
		vk::CommandBuffer commandBuffer)
	{
		commandBuffer.end();

		vk::SubmitInfo submitInfo {};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VK_CHECK(device.GetGraphicsQueue().submit(1, &submitInfo, nullptr));
		device.GetGraphicsQueue().waitIdle();

		device->freeCommandBuffers(device.GetCommandPool(), 1, &commandBuffer);
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

		VK_CHECK(device.createBuffer(&bufferInfo, nullptr, &buffer));

		vk::MemoryRequirements memRequirements;
		device.getBufferMemoryRequirements(buffer, &memRequirements);

		vk::MemoryAllocateInfo allocInfo = {};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		vk::MemoryAllocateFlagsInfo flagsInfo;
		if ((usage & vk::BufferUsageFlagBits::eShaderDeviceAddress) == vk::BufferUsageFlagBits::eShaderDeviceAddress)
			flagsInfo.flags = vk::MemoryAllocateFlagBits::eDeviceAddress;
		allocInfo.pNext = &flagsInfo;

		VK_CHECK(device.allocateMemory(&allocInfo, nullptr, &bufferMemory));
		device.bindBufferMemory(buffer, bufferMemory, 0);
	}

	void CreateBufferUnique(
		vk::PhysicalDevice physicalDevice,
		vk::Device device,
		vk::DeviceSize size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::UniqueBuffer& buffer,
		vk::UniqueDeviceMemory& bufferMemory)
	{
		vk::BufferCreateInfo bufferInfo = {};
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		buffer = device.createBufferUnique(bufferInfo);

		vk::MemoryRequirements memRequirements;
		device.getBufferMemoryRequirements(buffer.get(), &memRequirements);

		vk::MemoryAllocateInfo allocInfo = {};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		vk::MemoryAllocateFlagsInfo flagsInfo;
		if ((usage & vk::BufferUsageFlagBits::eShaderDeviceAddress) == vk::BufferUsageFlagBits::eShaderDeviceAddress)
			flagsInfo.flags = vk::MemoryAllocateFlagBits::eDeviceAddress;
		allocInfo.pNext = &flagsInfo;

		bufferMemory = device.allocateMemoryUnique(allocInfo);
		device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
	}

	void CopyBufferToBuffer(
		Device& device,
		vk::Buffer& srcBuffer,
		vk::Buffer& dstBuffer,
		vk::DeviceSize size)
	{
		vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(device);

		vk::BufferCopy copyRegion {};
		copyRegion.size = size;
		commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

		EndSingleTimeCommands(device, commandBuffer);
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

		VK_CHECK(device.createImage(&imageInfo, nullptr, &image));

		vk::MemoryRequirements memRequirements;
		device.getImageMemoryRequirements(image, &memRequirements);

		vk::MemoryAllocateInfo allocInfo {};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		VK_CHECK(device.allocateMemory(&allocInfo, nullptr, &imageMemory));
		device.bindImageMemory(image, imageMemory, 0);
	}

	void CreateImageUnique(
		vk::PhysicalDevice physicalDevice,
		vk::Device device,
		uint32_t width,
		uint32_t height,
		vk::Format format,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags properties,
		vk::UniqueImage& image,
		vk::UniqueDeviceMemory& imageMemory)
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

		image = device.createImageUnique(imageInfo);

		vk::MemoryRequirements memRequirements;
		device.getImageMemoryRequirements(image.get(), &memRequirements);

		vk::MemoryAllocateInfo allocInfo {};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		imageMemory = device.allocateMemoryUnique(allocInfo);
		device.bindImageMemory(image.get(), imageMemory.get(), 0);
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

		vk::ImageView imageView;
		VK_CHECK(device.createImageView(&viewInfo, nullptr, &imageView));

		return imageView;
	}

	vk::UniqueImageView CreateImageViewUnique(
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

		return device.createImageViewUnique(viewInfo);
	}

	void TransitionImageLayout(
		Device& device,
		vk::Image image,
		vk::Format /*format*/,
		vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout)
	{
		vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(device);

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

		EndSingleTimeCommands(device, commandBuffer);
	}

	void CopyBufferToImage(
		Device& device,
		vk::Buffer& buffer,
		vk::Image image,
		uint32_t width,
		uint32_t height)
	{
		vk::CommandBuffer commandBuffer = BeginSingleTimeCommands(device);

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

		EndSingleTimeCommands(device, commandBuffer);
	}

	void CreateDeviceLocalBuffer(
		Device& device,
		vk::PhysicalDevice physicalDevice,
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
			device.GetVkDevice(),
			bufferSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBuffer,
			stagingBufferMemory);

		void* data = nullptr;
		VK_CHECK(device->mapMemory(stagingBufferMemory, 0, bufferSize, {}, &data));
		memcpy(data, bufferData, static_cast<size_t>(bufferSize));
		device->unmapMemory(stagingBufferMemory);

		CreateBuffer(
			physicalDevice,
			device.GetVkDevice(),
			bufferSize,
			vk::BufferUsageFlagBits::eTransferDst | bufferUsage,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			dstBuffer,
			dstBufferMemory);

		CopyBufferToBuffer(
			device,
			stagingBuffer,
			dstBuffer,
			bufferSize);

		device->destroyBuffer(stagingBuffer);
		device->freeMemory(stagingBufferMemory);
	}

	template<>
	vk::PrimitiveTopology ToVulkanValue(DrawMode mode)
	{
		switch (mode)
		{
			case DrawMode::LINE_STRIP:
				return vk::PrimitiveTopology::eLineStrip;
			case DrawMode::LINES:
				return vk::PrimitiveTopology::eLineList;
			case DrawMode::TRIANGLE_FAN:
				return vk::PrimitiveTopology::eTriangleFan;
			case DrawMode::TRIANGLE_STRIP:
				return vk::PrimitiveTopology::eTriangleStrip;
			case DrawMode::TRIANGLES:
			default:
				return vk::PrimitiveTopology::eTriangleList;
		}
	}

	template<>
	vk::PolygonMode ToVulkanValue(PolygonMode mode)
	{
		switch (mode)
		{
			case PolygonMode::POINT:
				return vk::PolygonMode::ePoint;
			case PolygonMode::LINE:
				return vk::PolygonMode::eLine;
			case PolygonMode::FILL:
			default:
				return vk::PolygonMode::eFill;
		}
	}

	template<>
	vk::CompareOp ToVulkanValue(DepthFunction function)
	{
		switch (function)
		{
			case DepthFunction::ALWAYS:
				return vk::CompareOp::eAlways;
			case DepthFunction::NEVER:
				return vk::CompareOp::eNever;
			case DepthFunction::EQUAL:
				return vk::CompareOp::eEqual;
			case DepthFunction::GEQUAL:
				return vk::CompareOp::eGreaterOrEqual;
			case DepthFunction::GREATER:
				return vk::CompareOp::eGreater;
			case DepthFunction::LEQUAL:
				return vk::CompareOp::eLessOrEqual;
			case DepthFunction::NOT_EQUAL:
				return vk::CompareOp::eNotEqual;
			default:
			case DepthFunction::LESS:
				return vk::CompareOp::eLess;
		}
	}

	template<>
	vk::CullModeFlagBits ToVulkanValue(CullMode mode)
	{
		switch (mode)
		{
			case CullMode::BACK:
				return vk::CullModeFlagBits::eBack;
			case CullMode::FRONT:
				return vk::CullModeFlagBits::eFront;
			case CullMode::FRONT_AND_BACK:
				return vk::CullModeFlagBits::eFrontAndBack;
			default:
			case CullMode::NONE:
				return vk::CullModeFlagBits::eNone;
		}
	}

	template<>
	vk::FrontFace ToVulkanValue(FrontFace frontFace)
	{
		switch (frontFace)
		{
			case FrontFace::CCW:
				return vk::FrontFace::eCounterClockwise;
			default:
			case FrontFace::CW:
				return vk::FrontFace::eClockwise;
		}
	}

	template<>
	vk::AttachmentLoadOp ToVulkanValue(AttachmentLoadOp op)
	{
		switch (op)
		{
			case AttachmentLoadOp::CLEAR:
				return vk::AttachmentLoadOp::eClear;
			case AttachmentLoadOp::LOAD:
				return vk::AttachmentLoadOp::eLoad;
			default:
			case AttachmentLoadOp::DONT_CARE:
				return vk::AttachmentLoadOp::eDontCare;
		}
	}

	template<>
	vk::AttachmentStoreOp ToVulkanValue(AttachmentStoreOp op)
	{
		switch (op)
		{
			case AttachmentStoreOp::STORE:
				return vk::AttachmentStoreOp::eStore;
			default:
			case AttachmentStoreOp::DONT_CARE:
				return vk::AttachmentStoreOp::eDontCare;
		}
	}

	template<>
	vk::ImageLayout ToVulkanValue(ImageLayout layout)
	{
		switch (layout)
		{
			case ImageLayout::UNDEFINED:
				return vk::ImageLayout::eUndefined;
			case ImageLayout::COLOR_ATTACHMENT_OPTIMAL:
				return vk::ImageLayout::eColorAttachmentOptimal;
			case ImageLayout::DEPTH_ATTACHMMENT_OPTIMAL:
				return vk::ImageLayout::eDepthAttachmentOptimal;
			case ImageLayout::DEPTH_READONLY_OPTIMAL:
				return vk::ImageLayout::eDepthReadOnlyOptimal;
			case ImageLayout::STENCIL_ATTACHMENT_OPTIMAL:
				return vk::ImageLayout::eStencilAttachmentOptimal;
			case ImageLayout::STENCIL_READONLY_OPTIMAL:
				return vk::ImageLayout::eStencilReadOnlyOptimal;
			case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				return vk::ImageLayout::eDepthStencilAttachmentOptimal;
			case ImageLayout::DEPTH_STENCIL_READONLY_OPTIMAL:
				return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
			case ImageLayout::GENERAL:
				return vk::ImageLayout::eGeneral;
			default:
			case ImageLayout::PRESENT_SRC:
				return vk::ImageLayout::ePresentSrcKHR;
		}
	}

	template<>
	vk::ShaderStageFlagBits ToVulkanValue(ShaderType shaderType)
	{
		switch (shaderType)
		{
			case erm::ShaderType::VERTEX:
				return vk::ShaderStageFlagBits::eVertex;
			case erm::ShaderType::FRAGMENT:
				return vk::ShaderStageFlagBits::eFragment;
			case erm::ShaderType::RT_ANY_HIT:
				return vk::ShaderStageFlagBits::eAnyHitKHR;
			case erm::ShaderType::RT_CALLABLE:
				return vk::ShaderStageFlagBits::eCallableKHR;
			case erm::ShaderType::RT_CLOSEST_HIT:
				return vk::ShaderStageFlagBits::eClosestHitKHR;
			case erm::ShaderType::RT_INTERSECTION:
				return vk::ShaderStageFlagBits::eIntersectionKHR;
			case erm::ShaderType::RT_MISS:
				return vk::ShaderStageFlagBits::eMissKHR;
			case erm::ShaderType::RT_RAY_GEN:
				return vk::ShaderStageFlagBits::eRaygenKHR;
			default:
				ASSERT(false);
				return vk::ShaderStageFlagBits::eAll;
		}
	}

	template<>
	vk::DescriptorType ToVulkanValue(DescriptorType type)
	{
		switch (type)
		{
			case DescriptorType::UNIFORM_BUFFER:
				return vk::DescriptorType::eUniformBuffer;
			case DescriptorType::COMBINED_IMAGE_SAMPLER:
				return vk::DescriptorType::eCombinedImageSampler;
			case DescriptorType::STORAGE_IMAGE:
				return vk::DescriptorType::eStorageImage;
			case DescriptorType::STORAGE_BUFFER:
				return vk::DescriptorType::eStorageBuffer;
#ifdef ERM_RAY_TRACING_ENABLED
			case DescriptorType::ACCELERATION_STRUCTURE:
				return vk::DescriptorType::eAccelerationStructureKHR;
#endif
			default:
				ASSERT(false);
				return vk::DescriptorType::eUniformBuffer;
		}
	}

} // namespace erm::VkUtils
