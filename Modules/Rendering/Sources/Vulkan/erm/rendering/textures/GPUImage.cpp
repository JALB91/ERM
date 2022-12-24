#include "erm/rendering/textures/GPUImage.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/HostBuffer.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/resources/textures/CubeMap.h>
#include <erm/resources/textures/Texture.h>

#include <erm/utils/Utils.h>

#include <cmath>

namespace erm {

template<typename T>
constexpr vk::ImageViewType ToImageViewType()
{
	if constexpr (std::is_same_v<T, Texture>)
	{
		return vk::ImageViewType::e2D;
	}
	else if constexpr (std::is_same_v<T, CubeMap>)
	{
		return vk::ImageViewType::eCube;
	}
}

template<typename T>
GPUImage<T>::GPUImage(Device& device, const T& texture)
	: mDevice(device)
	, mTexture(texture)
	, mImage(nullptr)
	, mImageView(nullptr)
	, mImageMemory(nullptr)
	, mImageLayout(vk::ImageLayout::eUndefined)
	, mFormat(vk::Format::eR8G8B8A8Srgb)
	, mImageViewType(ToImageViewType<T>())
	, mOwnsImage(true)
{
	CreateTextureImage();
	GenerateMipmaps();
	CreateTextureImageView();
}

template<typename T>
GPUImage<T>::GPUImage(
	Device& device,
	const T& texture,
	vk::Image image,
	vk::ImageView imageView,
	vk::DeviceMemory imageMemory,
	vk::ImageLayout imageLayout,
	vk::Format format)
	: mDevice(device)
	, mTexture(texture)
	, mImage(image)
	, mImageView(imageView)
	, mImageMemory(imageMemory)
	, mImageLayout(imageLayout)
	, mFormat(format)
	, mImageViewType(ToImageViewType<T>())
	, mOwnsImage(false)
{}

template<typename T>
GPUImage<T>::~GPUImage()
{
	if (mOwnsImage && mImage)
		mDevice->destroyImage(mImage);
	if (mImageView)
		mDevice->destroyImageView(mImageView);
	if (mImageMemory)
		mDevice->freeMemory(mImageMemory);
}

template<>
void GPUImage<Texture>::CreateTextureImage()
{
	const vk::DeviceSize imageSize = mTexture.GetWidth() * mTexture.GetHeight() * 4;

	HostBuffer stagingBuffer(mDevice, imageSize, BufferUsage::TRANSFER_SRC);
	stagingBuffer.Update(mTexture.GetBuffer());

	vk::ImageCreateInfo imageInfo {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = mTexture.GetWidth();
	imageInfo.extent.height = mTexture.GetHeight();
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mTexture.GetMipLevels();
	imageInfo.arrayLayers = mTexture.GetArrayLayers();
	imageInfo.format = GetImageFormat();
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout = GetImageLayout();
	imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;

	VkUtils::CreateImage(
		mDevice.GetVkPhysicalDevice(),
		mDevice.GetVkDevice(),
		imageInfo,
		mImage,
		mImageMemory,
		MemoryProperty::DEVICE_LOCAL);

	TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal);

	VkUtils::CopyBufferToImage(
		mDevice,
		stagingBuffer.GetBuffer(),
		GetImage(),
		static_cast<uint32_t>(mTexture.GetWidth()),
		static_cast<uint32_t>(mTexture.GetHeight()));
}

template<>
void GPUImage<CubeMap>::CreateTextureImage()
{
	const vk::DeviceSize imageSize = mTexture.GetWidth() * mTexture.GetHeight() * 4;
	HostBuffer stagingBuffer (mDevice, imageSize * CubeMap::kFaces.size(), BufferUsage::TRANSFER_SRC);

	for (size_t i = 0; i < CubeMap::kFaces.size(); ++i)
	{
		stagingBuffer.Update(mTexture.GetBuffers()[i], BufferInfo(i * imageSize, imageSize));
	}
	
	vk::ImageCreateInfo imageInfo {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = mTexture.GetWidth();
	imageInfo.extent.height = mTexture.GetHeight();
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mTexture.GetMipLevels();
	imageInfo.arrayLayers = mTexture.GetArrayLayers();
	imageInfo.format = GetImageFormat();
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout = GetImageLayout();
	imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;
	imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;

	VkUtils::CreateImage(
		mDevice.GetVkPhysicalDevice(),
		mDevice.GetVkDevice(),
		imageInfo,
		mImage,
		mImageMemory,
		MemoryProperty::DEVICE_LOCAL);

	TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal);

	VkUtils::CopyBufferToImage(
		mDevice,
		stagingBuffer.GetBuffer(),
		GetImage(),
		static_cast<uint32_t>(mTexture.GetWidth()),
		static_cast<uint32_t>(mTexture.GetHeight()),
		mTexture.GetArrayLayers());
}

template<typename T>
void GPUImage<T>::TransitionImageLayout(vk::ImageLayout newLayout)
{
	VkUtils::TransitionImageLayout(
		mDevice,
		GetImage(),
		GetImageLayout(),
		newLayout,
		mTexture.GetMipLevels(),
		mTexture.GetArrayLayers());

	mImageLayout = newLayout;
}

template<typename T>
void GPUImage<T>::GenerateMipmaps()
{
	vk::FormatProperties properties = mDevice.GetVkPhysicalDevice().getFormatProperties(GetImageFormat());

	if (!(properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear) || mTexture.GetMipLevels() <= 1)
	{
//		TODO: Damiano
//		image.SetMipLevels(1);
		ERM_ASSERT(mTexture.GetMipLevels() == 1);
		TransitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
		return;
	}

	vk::CommandBuffer commandBuffer = VkUtils::BeginSingleTimeCommands(mDevice);

	vk::ImageMemoryBarrier barrier {};
	barrier.image = GetImage();
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	uint32_t mipWidth = mTexture.GetWidth();
	uint32_t mipHeight = mTexture.GetHeight();

	for (uint32_t i = 1; i < mTexture.GetMipLevels(); ++i)
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

		commandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eTransfer,
			{},
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier);

		vk::ImageBlit blit {};
		blit.srcOffsets[0] = vk::Offset3D {0, 0, 0};
		blit.srcOffsets[1] = vk::Offset3D {static_cast<int32_t>(mipWidth), static_cast<int32_t>(mipHeight), 1};
		blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = vk::Offset3D {0, 0, 0};
		blit.dstOffsets[1] = vk::Offset3D {static_cast<int32_t>(mipWidth > 1 ? mipWidth / 2 : 1), static_cast<int32_t>(mipHeight > 1 ? mipHeight / 2 : 1), 1};
		blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		commandBuffer.blitImage(
			GetImage(),
			vk::ImageLayout::eTransferSrcOptimal,
			GetImage(),
			vk::ImageLayout::eTransferDstOptimal,
			1,
			&blit,
			vk::Filter::eLinear);

		barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		commandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eFragmentShader,
			{},
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier);

		if (mipWidth > 1)
			mipWidth /= 2;
		if (mipHeight > 1)
			mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mTexture.GetMipLevels() - 1;
	barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

	commandBuffer.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eFragmentShader,
		{},
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);

	VkUtils::EndSingleTimeCommands(mDevice, commandBuffer);

	mImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
}

template<typename T>
void GPUImage<T>::CreateTextureImageView()
{
	vk::ImageViewCreateInfo viewInfo {};
	viewInfo.image = GetImage();
	viewInfo.viewType = GetImageViewType();
	viewInfo.format = GetImageFormat();
	viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mTexture.GetMipLevels();
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = mTexture.GetArrayLayers();

	mImageView = VkUtils::CreateImageView(mDevice.GetVkDevice(), viewInfo);
}

} // namespace erm
