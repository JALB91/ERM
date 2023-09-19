#include "erm/rendering/textures/GPUImage.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/HostBuffer.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/assets/textures/CubeMap.h>
#include <erm/assets/textures/Texture.h>

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
GPUImage<T>::GPUImage(Device& device, T& texture)
	: mDevice(device)
	, mTexture(texture)
	, mMipLevels(0)
	, mArrayLayers(0)
	, mImage(nullptr)
	, mImageView(nullptr)
	, mImageMemory(nullptr)
	, mImageLayout(vk::ImageLayout::eUndefined)
	, mFormat(vk::Format::eR8G8B8A8Srgb)
	, mImageViewType(ToImageViewType<T>())
	, mOwnsImage(true)
{
	createTextureImage();
	generateMipmaps();
	createTextureImageView();
}

template<typename T>
GPUImage<T>::GPUImage(
	Device& device,
	T& texture,
	u32 mipLevels,
	u32 arrayLayers,
	vk::Image image,
	vk::ImageView imageView,
	vk::DeviceMemory imageMemory,
	vk::ImageLayout imageLayout,
	vk::Format format)
	: mDevice(device)
	, mTexture(texture)
	, mMipLevels(mipLevels)
	, mArrayLayers(arrayLayers)
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
	{
		mDevice->destroyImage(mImage);
	}
	if (mImageView)
	{
		mDevice->destroyImageView(mImageView);
	}
	if (mImageMemory)
	{
		mDevice->freeMemory(mImageMemory);
	}
}

template<>
void GPUImage<Texture>::createTextureImage()
{
	const vk::DeviceSize imageSize = mTexture.mWidth * mTexture.mHeight * 4;

	HostBuffer stagingBuffer(mDevice, imageSize, BufferUsage::TRANSFER_SRC);
	stagingBuffer.update(mTexture.mBuffer);

	vk::ImageCreateInfo imageInfo {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = mTexture.mWidth;
	imageInfo.extent.height = mTexture.mHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mMipLevels;
	imageInfo.arrayLayers = mArrayLayers;
	imageInfo.format = getImageFormat();
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout = getImageLayout();
	imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;

	VkUtils::createImage(
		mDevice.getVkPhysicalDevice(),
		mDevice.getVkDevice(),
		imageInfo,
		mImage,
		mImageMemory,
		MemoryProperty::DEVICE_LOCAL);

	transitionImageLayout(vk::ImageLayout::eTransferDstOptimal);

	VkUtils::copyBufferToImage(
		mDevice,
		stagingBuffer.getBuffer(),
		getImage(),
		static_cast<u32>(mTexture.mWidth),
		static_cast<u32>(mTexture.mHeight));
}

template<>
void GPUImage<CubeMap>::createTextureImage()
{
	const vk::DeviceSize imageSize = mTexture.mWidth * mTexture.mHeight * mTexture.mBPP;
	HostBuffer stagingBuffer (mDevice, imageSize * CubeMap::kFaces.size(), BufferUsage::TRANSFER_SRC);

	for (u8 i = 0; i < CubeMap::kFaces.size(); ++i)
	{
		stagingBuffer.update(mTexture.mBuffers[i], BufferInfo(i * imageSize, imageSize));
	}
	
	vk::ImageCreateInfo imageInfo {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = mTexture.mWidth;
	imageInfo.extent.height = mTexture.mHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mMipLevels;
	imageInfo.arrayLayers = mArrayLayers;
	imageInfo.format = getImageFormat();
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout = getImageLayout();
	imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;
	imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;

	VkUtils::createImage(
		mDevice.getVkPhysicalDevice(),
		mDevice.getVkDevice(),
		imageInfo,
		mImage,
		mImageMemory,
		MemoryProperty::DEVICE_LOCAL);

	transitionImageLayout(vk::ImageLayout::eTransferDstOptimal);

	VkUtils::copyBufferToImage(
		mDevice,
		stagingBuffer.getBuffer(),
		getImage(),
		static_cast<u32>(mTexture.mWidth),
		static_cast<u32>(mTexture.mHeight),
		mArrayLayers);
}

template<typename T>
void GPUImage<T>::transitionImageLayout(vk::ImageLayout newLayout)
{
	VkUtils::transitionImageLayout(
		mDevice,
		getImage(),
		getImageLayout(),
		newLayout,
		mMipLevels,
		mArrayLayers);

	mImageLayout = newLayout;
}

template<typename T>
void GPUImage<T>::generateMipmaps()
{
	vk::FormatProperties properties = mDevice.getVkPhysicalDevice().getFormatProperties(getImageFormat());

	if (!(properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear) || mTexture.getMipLevels() <= 1)
	{
		mTexture.setMipLevels(1);
		transitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
		return;
	}

	vk::CommandBuffer commandBuffer = VkUtils::beginSingleTimeCommands(mDevice);

	vk::ImageMemoryBarrier barrier {};
	barrier.image = getImage();
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	u32 mipWidth = mTexture.getWidth();
	u32 mipHeight = mTexture.getHeight();

	for (u32 i = 1; i < mMipLevels; ++i)
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
		blit.srcOffsets[1] = vk::Offset3D {static_cast<i32>(mipWidth), static_cast<i32>(mipHeight), 1};
		blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = vk::Offset3D {0, 0, 0};
		blit.dstOffsets[1] = vk::Offset3D {static_cast<i32>(mipWidth > 1 ? mipWidth / 2 : 1), static_cast<i32>(mipHeight > 1 ? mipHeight / 2 : 1), 1};
		blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		commandBuffer.blitImage(
			getImage(),
			vk::ImageLayout::eTransferSrcOptimal,
			getImage(),
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
		{
			mipWidth /= 2;
		}
		if (mipHeight > 1)
		{
			mipHeight /= 2;
		}
	}

	barrier.subresourceRange.baseMipLevel = mMipLevels - 1;
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

	VkUtils::endSingleTimeCommands(mDevice, commandBuffer);

	mImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
}

template<typename T>
void GPUImage<T>::createTextureImageView()
{
	vk::ImageViewCreateInfo viewInfo {};
	viewInfo.image = getImage();
	viewInfo.viewType = getImageViewType();
	viewInfo.format = getImageFormat();
	viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mMipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = mArrayLayers;

	mImageView = VkUtils::createImageView(mDevice.getVkDevice(), viewInfo);
}

} // namespace erm
