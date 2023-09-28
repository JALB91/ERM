#include "erm/rendering/utils/VkUtils.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/QueueFamilyIndices.h"
#include "erm/rendering/data_structs/SwapChainSupportDetails.h"
#include "erm/rendering/enums/AttachmentLoadOp.h"
#include "erm/rendering/enums/AttachmentStoreOp.h"
#include "erm/rendering/enums/CullMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/DescriptorType.h"
#include "erm/rendering/enums/DrawMode.h"
#include "erm/rendering/enums/FrontFace.h"
#include "erm/rendering/enums/ImageLayout.h"
#include "erm/rendering/enums/PolygonMode.h"

#include <erm/assets/enums/ShaderType.h>

#include <erm/utils/Utils.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <stdexcept>

namespace erm::VkUtils {

template<>
vk::PrimitiveTopology toVulkanValue(DrawMode mode)
{
	switch (mode)
	{
		case DrawMode::LINE_STRIP:
		{
			return vk::PrimitiveTopology::eLineStrip;
		}
		case DrawMode::LINES:
		{
			return vk::PrimitiveTopology::eLineList;
		}
		case DrawMode::TRIANGLE_FAN:
		{
			return vk::PrimitiveTopology::eTriangleFan;
		}
		case DrawMode::TRIANGLE_STRIP:
		{
			return vk::PrimitiveTopology::eTriangleStrip;
		}
		case DrawMode::TRIANGLES:
		{
			return vk::PrimitiveTopology::eTriangleList;
		}
		default:
		{
			ERM_ASSERT(false);
			return vk::PrimitiveTopology::eTriangleList;
		}
	}
}

template<>
vk::PolygonMode toVulkanValue(PolygonMode mode)
{
	switch (mode)
	{
		case PolygonMode::POINT:
		{
			return vk::PolygonMode::ePoint;
		}
		case PolygonMode::LINE:
		{
			return vk::PolygonMode::eLine;
		}
		case PolygonMode::FILL:
		{
			return vk::PolygonMode::eFill;
		}
		default:
		{
			ERM_ASSERT(false);
			return vk::PolygonMode::eFill;
		}
	}
}

template<>
vk::CompareOp toVulkanValue(DepthFunction function)
{
	switch (function)
	{
		case DepthFunction::ALWAYS:
		{
			return vk::CompareOp::eAlways;
		}
		case DepthFunction::NEVER:
		{
			return vk::CompareOp::eNever;
		}
		case DepthFunction::EQUAL:
		{
			return vk::CompareOp::eEqual;
		}
		case DepthFunction::GEQUAL:
		{
			return vk::CompareOp::eGreaterOrEqual;
		}
		case DepthFunction::GREATER:
		{
			return vk::CompareOp::eGreater;
		}
		case DepthFunction::LEQUAL:
		{
			return vk::CompareOp::eLessOrEqual;
		}
		case DepthFunction::NOT_EQUAL:
		{
			return vk::CompareOp::eNotEqual;
		}
		case DepthFunction::LESS:
		{
			return vk::CompareOp::eLess;
		}
		default:
		{
			ERM_ASSERT(false);
			return vk::CompareOp::eLess;
		}
	}
}

template<>
vk::CullModeFlagBits toVulkanValue(CullMode mode)
{
	switch (mode)
	{
		case CullMode::BACK:
		{
			return vk::CullModeFlagBits::eBack;
		}
		case CullMode::FRONT:
		{
			return vk::CullModeFlagBits::eFront;
		}
		case CullMode::FRONT_AND_BACK:
		{
			return vk::CullModeFlagBits::eFrontAndBack;
		}
		case CullMode::NONE:
		{
			return vk::CullModeFlagBits::eNone;
		}
		default:
		{
			ERM_ASSERT(false);
			return vk::CullModeFlagBits::eNone;
		}
	}
}

template<>
vk::FrontFace toVulkanValue(FrontFace frontFace)
{
	switch (frontFace)
	{
		case FrontFace::CCW:
		{
			return vk::FrontFace::eCounterClockwise;
		}
		case FrontFace::CW:
		{
			return vk::FrontFace::eClockwise;
		}
		default:
		{
			ERM_ASSERT(false);
			return vk::FrontFace::eClockwise;
		}
	}
}

template<>
vk::AttachmentLoadOp toVulkanValue(AttachmentLoadOp op)
{
	switch (op)
	{
		case AttachmentLoadOp::CLEAR:
		{
			return vk::AttachmentLoadOp::eClear;
		}
		case AttachmentLoadOp::LOAD:
		{
			return vk::AttachmentLoadOp::eLoad;
		}
		case AttachmentLoadOp::DONT_CARE:
		{
			return vk::AttachmentLoadOp::eDontCare;
		}
		default:
		{
			ERM_ASSERT(false);
			return vk::AttachmentLoadOp::eDontCare;
		}
	}
}

template<>
vk::AttachmentStoreOp toVulkanValue(AttachmentStoreOp op)
{
	switch (op)
	{
		case AttachmentStoreOp::STORE:
		{
			return vk::AttachmentStoreOp::eStore;
		}
		case AttachmentStoreOp::DONT_CARE:
		{
			return vk::AttachmentStoreOp::eDontCare;
		}
		default:
		{
			ERM_ASSERT(false);
			return vk::AttachmentStoreOp::eDontCare;
		}
	}
}

template<>
vk::ImageLayout toVulkanValue(ImageLayout layout)
{
	switch (layout)
	{
		case ImageLayout::UNDEFINED:
		{
			return vk::ImageLayout::eUndefined;
		}
		case ImageLayout::COLOR_ATTACHMENT_OPTIMAL:
		{
			return vk::ImageLayout::eColorAttachmentOptimal;
		}
		case ImageLayout::DEPTH_ATTACHMENT_OPTIMAL:
		{
			return vk::ImageLayout::eDepthAttachmentOptimal;
		}
		case ImageLayout::DEPTH_READONLY_OPTIMAL:
		{
			return vk::ImageLayout::eDepthReadOnlyOptimal;
		}
		case ImageLayout::STENCIL_ATTACHMENT_OPTIMAL:
		{
			return vk::ImageLayout::eStencilAttachmentOptimal;
		}
		case ImageLayout::STENCIL_READONLY_OPTIMAL:
		{
			return vk::ImageLayout::eStencilReadOnlyOptimal;
		}
		case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			return vk::ImageLayout::eDepthStencilAttachmentOptimal;
		}
		case ImageLayout::DEPTH_STENCIL_READONLY_OPTIMAL:
		{
			return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
		}
		case ImageLayout::GENERAL:
		{
			return vk::ImageLayout::eGeneral;
		}
		case ImageLayout::SHADER_READ_ONLY_OPTIMAL:
		{
			return vk::ImageLayout::eShaderReadOnlyOptimal;
		}
		case ImageLayout::TRANSFER_DST_OPTIMAL:
		{
			return vk::ImageLayout::eTransferDstOptimal;
		}
		case ImageLayout::PRESENT_SRC:
		{
			return vk::ImageLayout::ePresentSrcKHR;
		}
		default:
		{
			ERM_ASSERT(false);
			return vk::ImageLayout::eUndefined;
		}
	}
}

template<>
vk::ShaderStageFlagBits toVulkanValue(ShaderType shaderType)
{
	switch (shaderType)
	{
		case erm::ShaderType::VERTEX:
		{
			return vk::ShaderStageFlagBits::eVertex;
		}
		case erm::ShaderType::FRAGMENT:
		{
			return vk::ShaderStageFlagBits::eFragment;
		}
#ifdef ERM_RAY_TRACING_ENABLED
		case erm::ShaderType::RT_ANY_HIT:
		{
			return vk::ShaderStageFlagBits::eAnyHitKHR;
		}
		case erm::ShaderType::RT_CALLABLE:
		{
			return vk::ShaderStageFlagBits::eCallableKHR;
		}
		case erm::ShaderType::RT_CLOSEST_HIT:
		{
			return vk::ShaderStageFlagBits::eClosestHitKHR;
		}
		case erm::ShaderType::RT_INTERSECTION:
		{
			return vk::ShaderStageFlagBits::eIntersectionKHR;
		}
		case erm::ShaderType::RT_MISS:
		{
			return vk::ShaderStageFlagBits::eMissKHR;
		}
		case erm::ShaderType::RT_RAY_GEN:
		{
			return vk::ShaderStageFlagBits::eRaygenKHR;
		}
#endif
		default:
		{
			ERM_ASSERT(false);
			return vk::ShaderStageFlagBits::eAll;
		}
	}
}

template<>
vk::DescriptorType toVulkanValue(DescriptorType type)
{
	switch (type)
	{
		case DescriptorType::UNIFORM_BUFFER:
		{
			return vk::DescriptorType::eUniformBuffer;
		}
		case DescriptorType::COMBINED_IMAGE_SAMPLER:
		{
			return vk::DescriptorType::eCombinedImageSampler;
		}
		case DescriptorType::STORAGE_IMAGE:
		{
			return vk::DescriptorType::eStorageImage;
		}
		case DescriptorType::STORAGE_BUFFER:
		{
			return vk::DescriptorType::eStorageBuffer;
		}
#ifdef ERM_RAY_TRACING_ENABLED
		case DescriptorType::ACCELERATION_STRUCTURE:
		{
			return vk::DescriptorType::eAccelerationStructureKHR;
		}
#endif
		default:
		{
			ERM_ASSERT(false);
			return vk::DescriptorType::eUniformBuffer;
		}
	}
}

template<>
vk::BufferUsageFlagBits toVulkanValue(BufferUsage usage)
{
	switch (usage)
	{
		case BufferUsage::TRANSFER_SRC:
		{
			return vk::BufferUsageFlagBits::eTransferSrc;
		}
		case BufferUsage::TRANSFER_DST:
		{
			return vk::BufferUsageFlagBits::eTransferDst;
		}
		case BufferUsage::UNIFORM_BUFFER:
		{
			return vk::BufferUsageFlagBits::eUniformBuffer;
		}
		case BufferUsage::STORAGE_BUFFER:
		{
			return vk::BufferUsageFlagBits::eStorageBuffer;
		}
		case BufferUsage::INDEX_BUFFER:
		{
			return vk::BufferUsageFlagBits::eIndexBuffer;
		}
		case BufferUsage::VERTEX_BUFFER:
		{
			return vk::BufferUsageFlagBits::eVertexBuffer;
		}
		case BufferUsage::SHADER_DEVICE_ADDRESS:
		{
			return vk::BufferUsageFlagBits::eShaderDeviceAddress;
		}
#ifdef ERM_RAY_TRACING_ENABLED
		case BufferUsage::ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY:
		{
			return vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR;
		}
		case BufferUsage::ACCELERATION_STRUCTURE_STORAGE:
		{
			return vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR;
		}
		case BufferUsage::SHADER_BINDING_TABLE:
		{
			return vk::BufferUsageFlagBits::eShaderBindingTableKHR;
		}
#endif
		default:
		{
			ERM_ASSERT(false);
			return vk::BufferUsageFlagBits::eTransferSrc;
		}
	}
}

template<>
vk::MemoryPropertyFlagBits toVulkanValue(MemoryProperty property)
{
	switch (property)
	{
		case MemoryProperty::DEVICE_LOCAL:
		{
			return vk::MemoryPropertyFlagBits::eDeviceLocal;
		}
		case MemoryProperty::HOST_VISIBLE:
		{
			return vk::MemoryPropertyFlagBits::eHostVisible;
		}
		case MemoryProperty::HOST_COHERENT:
		{
			return vk::MemoryPropertyFlagBits::eHostCoherent;
		}
		case MemoryProperty::HOST_CACHED:
		{
			return vk::MemoryPropertyFlagBits::eHostCached;
		}
		case MemoryProperty::LAZILY_ALLOCATED:
		{
			return vk::MemoryPropertyFlagBits::eLazilyAllocated;
		}
		case MemoryProperty::PROTECTED:
		{
			return vk::MemoryPropertyFlagBits::eProtected;
		}
		default:
		{
			ERM_ASSERT(false);
			return vk::MemoryPropertyFlagBits::eDeviceLocal;
		}
	}
}

template<>
vk::BufferUsageFlags toVulkanValue(BufferUsageFlags flags)
{
	vk::BufferUsageFlags result;

	for (u32 i = 1; i < static_cast<u32>(BufferUsage::COUNT); i *= 2)
	{
		if ((flags & i) == i)
		{
			result |= toVulkanValue<vk::BufferUsageFlagBits>(static_cast<BufferUsage>(i));
		}
	}

	return result;
}

template<>
vk::MemoryPropertyFlags toVulkanValue(MemoryPropertyFlags flags)
{
	vk::MemoryPropertyFlags result;

	for (u32 i = 1; i < static_cast<u32>(MemoryProperty::COUNT); i *= 2)
	{
		if ((flags & i) == i)
		{
			result |= toVulkanValue<vk::MemoryPropertyFlagBits>(static_cast<MemoryProperty>(i));
		}
	}

	return result;
}

QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
	QueueFamilyIndices indices;

	auto queueFamilies = device.getQueueFamilyProperties();

	for (u32 i = 0; i < static_cast<u32>(queueFamilies.size()); ++i)
	{
		if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.mGraphicsFamily = i;
		}
		
		vk::Bool32 surfaceSupport;
		ERM_VK_CHECK_AND_ASSIGN(surfaceSupport, device.getSurfaceSupportKHR(i, surface));

		if (surfaceSupport)
		{
			indices.mPresentFamily = i;
		}
	}

	return indices;
}

SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
	SwapChainSupportDetails details;
	ERM_VK_CHECK_AND_ASSIGN(details.mCapabilities, device.getSurfaceCapabilitiesKHR(surface));
	ERM_VK_CHECK_AND_ASSIGN(details.mFormats, device.getSurfaceFormatsKHR(surface));
	ERM_VK_CHECK_AND_ASSIGN(details.mPresentModes, device.getSurfacePresentModesKHR(surface));
	return details;
}

bool checkDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*>& deviceExtensions)
{
	std::vector<vk::ExtensionProperties> availableExtensions;
	ERM_VK_CHECK_AND_ASSIGN(availableExtensions, device.enumerateDeviceExtensionProperties());
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool isDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface, const std::vector<const char*>& deviceExtensions)
{
	const auto deviceProperties = device.getProperties();
	const bool isSuitable = findQueueFamilies(device, surface).isComplete() && checkDeviceExtensionSupport(device, deviceExtensions);

	bool swapChainAdequate = false;
	if (isSuitable)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
		swapChainAdequate = !swapChainSupport.mFormats.empty() && !swapChainSupport.mPresentModes.empty();
	}

	ERM_LOG_INFO("Checking compatibility of: %s -> %s", deviceProperties.deviceName, (isSuitable && swapChainAdequate) ? "true" : "false");

	auto supportedFeatures = device.getFeatures();

	return isSuitable && swapChainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.fillModeNonSolid;
}

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
#if defined(ERM_RAY_TRACING_ENABLED)
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

vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == vk::PresentModeKHR::eMailbox)
		{
			return availablePresentMode;
		}
	}
	return vk::PresentModeKHR::eFifo;
}

vk::Format findSupportedDepthFormat(
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
	
	return vk::Format::eUndefined;
}

vk::Format findDepthFormat(vk::PhysicalDevice physicalDevice)
{
	return findSupportedDepthFormat(
		physicalDevice,
		{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

vk::CommandBuffer beginSingleTimeCommands(Device& device)
{
	vk::CommandBufferAllocateInfo allocInfo {};
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandPool = device.getCommandPool();
	allocInfo.commandBufferCount = 1;

	vk::CommandBuffer commandBuffer;
	ERM_VK_CHECK(device->allocateCommandBuffers(&allocInfo, &commandBuffer));

	vk::CommandBufferBeginInfo beginInfo {};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	ERM_VK_CHECK(commandBuffer.begin(beginInfo));

	return commandBuffer;
}

void endSingleTimeCommands(
	Device& device,
	vk::CommandBuffer commandBuffer)
{
	ERM_VK_CHECK(commandBuffer.end());

	vk::SubmitInfo submitInfo {};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	ERM_VK_CHECK(device.getGraphicsQueue().submit(1, &submitInfo, nullptr));
	ERM_VK_CHECK(device.getGraphicsQueue().waitIdle());

	device->freeCommandBuffers(device.getCommandPool(), 1, &commandBuffer);
}

vk::Extent2D chooseSwapExtent(
	const vk::SurfaceCapabilitiesKHR& capabilities,
	u32 fallbackWidth,
	u32 fallbackHeight)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		vk::Extent2D actualExtent = {fallbackWidth, fallbackHeight};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

u32 findMemoryType(
	vk::PhysicalDevice physicalDevice,
	u32 typeFilter,
	MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties;
	physicalDevice.getMemoryProperties(&memProperties);

	const auto vkProperties = toVulkanValue<vk::MemoryPropertyFlags>(properties);

	for (u32 i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & vkProperties) == vkProperties)
		{
			return i;
		}
	}

	return 0;
}

void createBuffer(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	vk::DeviceSize size,
	vk::Buffer& buffer,
	vk::DeviceMemory& bufferMemory,
	BufferUsageFlags usage,
	MemoryPropertyFlags properties)
{
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size = size;
	bufferInfo.usage = toVulkanValue<vk::BufferUsageFlags>(usage);
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	ERM_VK_CHECK(device.createBuffer(&bufferInfo, nullptr, &buffer));

	vk::MemoryRequirements memRequirements;
	device.getBufferMemoryRequirements(buffer, &memRequirements);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	vk::MemoryAllocateFlagsInfo flagsInfo;
	if ((usage & BufferUsage::SHADER_DEVICE_ADDRESS) == BufferUsage::SHADER_DEVICE_ADDRESS)
	{	
		flagsInfo.flags = vk::MemoryAllocateFlagBits::eDeviceAddress;
	}
	allocInfo.pNext = &flagsInfo;

	ERM_VK_CHECK(device.allocateMemory(&allocInfo, nullptr, &bufferMemory));
	ERM_VK_CHECK(device.bindBufferMemory(buffer, bufferMemory, 0));
}

void createBufferUnique(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	vk::DeviceSize size,
	vk::UniqueBuffer& buffer,
	vk::UniqueDeviceMemory& bufferMemory,
	BufferUsageFlags usage,
	MemoryPropertyFlags properties)
{
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size = size;
	bufferInfo.usage = toVulkanValue<vk::BufferUsageFlags>(usage);
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;

	ERM_VK_CHECK_AND_ASSIGN(buffer, device.createBufferUnique(bufferInfo));

	vk::MemoryRequirements memRequirements;
	device.getBufferMemoryRequirements(buffer.get(), &memRequirements);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	vk::MemoryAllocateFlagsInfo flagsInfo;
	if ((usage & BufferUsage::SHADER_DEVICE_ADDRESS) == BufferUsage::SHADER_DEVICE_ADDRESS)
	{
		flagsInfo.flags = vk::MemoryAllocateFlagBits::eDeviceAddress;
	}
	allocInfo.pNext = &flagsInfo;

	ERM_VK_CHECK_AND_ASSIGN(bufferMemory, device.allocateMemoryUnique(allocInfo));
	ERM_VK_CHECK(device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0));
}

void copyBufferToBuffer(
	Device& device,
	vk::Buffer& srcBuffer,
	vk::Buffer& dstBuffer,
	vk::DeviceSize size)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands(device);

	vk::BufferCopy copyRegion {};
	copyRegion.size = size;
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(device, commandBuffer);
}

void createImage(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	const vk::ImageCreateInfo& createInfo,
	vk::Image& image,
	vk::DeviceMemory& imageMemory,
	MemoryPropertyFlags properties)
{
	ERM_VK_CHECK(device.createImage(&createInfo, nullptr, &image));

	vk::MemoryRequirements memRequirements;
	device.getImageMemoryRequirements(image, &memRequirements);

	vk::MemoryAllocateInfo allocInfo {};
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	ERM_VK_CHECK(device.allocateMemory(&allocInfo, nullptr, &imageMemory));
	ERM_VK_CHECK(device.bindImageMemory(image, imageMemory, 0));
}

void createImageUnique(
	vk::PhysicalDevice physicalDevice,
	vk::Device device,
	const vk::ImageCreateInfo& createInfo,
	vk::UniqueImage& image,
	vk::UniqueDeviceMemory& imageMemory,
	MemoryPropertyFlags properties)
{
	ERM_VK_CHECK_AND_ASSIGN(image, device.createImageUnique(createInfo));

	vk::MemoryRequirements memRequirements;
	device.getImageMemoryRequirements(image.get(), &memRequirements);

	vk::MemoryAllocateInfo allocInfo {};
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	ERM_VK_CHECK_AND_ASSIGN(imageMemory, device.allocateMemoryUnique(allocInfo));
	ERM_VK_CHECK(device.bindImageMemory(image.get(), imageMemory.get(), 0));
}

vk::ImageView createImageView(
	vk::Device device,
	const vk::ImageViewCreateInfo& createInfo)
{
	vk::ImageView imageView;
	ERM_VK_CHECK(device.createImageView(&createInfo, nullptr, &imageView));
	return imageView;
}

vk::UniqueImageView createImageViewUnique(
	vk::Device device,
	const vk::ImageViewCreateInfo& createInfo)
{
	ERM_VK_CHECK_AND_RETURN(device.createImageViewUnique(createInfo));
}

void transitionImageLayout(
	Device& device,
	vk::Image image,
	vk::ImageLayout oldLayout,
	vk::ImageLayout newLayout,
	u32 mipLevels /* = 1 */,
	u32 layerCount /* = 1*/)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands(device);

	vk::ImageMemoryBarrier barrier {};
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layerCount;

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
		ERM_ASSERT(false);
		endSingleTimeCommands(device, commandBuffer);
		return;
	}

	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, 0, nullptr, 0, nullptr, 1, &barrier);

	endSingleTimeCommands(device, commandBuffer);
}

void copyBufferToImage(
	Device& device,
	vk::Buffer buffer,
	vk::Image image,
	u32 width,
	u32 height,
	u32 layerCount /* = 1*/)
{
	vk::CommandBuffer commandBuffer = beginSingleTimeCommands(device);

	vk::BufferImageCopy region {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = layerCount;
	region.imageOffset = vk::Offset3D(0, 0, 0);
	region.imageExtent = vk::Extent3D(width, height, 1);

	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

	endSingleTimeCommands(device, commandBuffer);
}

} // namespace erm::VkUtils
