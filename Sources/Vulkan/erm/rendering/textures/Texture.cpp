#include "erm/rendering/textures/Texture.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/HostBuffer.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <stb_image.h>

namespace erm {

Texture::Texture(Device& device)
	: IAsset("", "")
	, mDevice(device)
	, mLocalBuffer(nullptr)
	, mWidth(0)
	, mHeight(0)
	, mBPP(0)
	, mMipLevels(0)
	, mArrayLayers(0)
	, mImage(nullptr)
	, mImageView(nullptr)
	, mImageMemory(nullptr)
	, mImageLayout(vk::ImageLayout::eUndefined)
	, mFormat(vk::Format::eR8G8B8A8Srgb)
	, mImageViewType(vk::ImageViewType::e2D)
{}

Texture::~Texture()
{
	if (mImage)
		mDevice->destroyImage(mImage);
	if (mImageView)
		mDevice->destroyImageView(mImageView);
	if (mImageMemory)
		mDevice->freeMemory(mImageMemory);
}

void Texture::InitFromFile(const char* path)
{
	mPath = path;
	ERM_ASSERT(!mImage && !mImageView && !mImageMemory);
	CreateTextureImage();
	GenerateMipmaps();
	CreateTextureImageView();
}

void Texture::InitWithData(
	const char* name,
	vk::Image image,
	vk::ImageView imageView,
	vk::DeviceMemory imageMemory,
	vk::ImageLayout imageLayout,
	vk::Format format,
	uint32_t width,
	uint32_t height)
{
	mName = name;
	mImage = image;
	mImageView = imageView;
	mImageMemory = imageMemory;
	mImageLayout = imageLayout;
	mFormat = format;
	mWidth = width;
	mHeight = height;
	mMipLevels = 1;
	mArrayLayers = 1;
}

void Texture::TransitionImageLayout(vk::ImageLayout newLayout)
{
	VkUtils::TransitionImageLayout(
		mDevice,
		GetImage(),
		GetImageLayout(),
		newLayout,
		GetMipLevels(),
		GetArrayLayers());

	mImageLayout = newLayout;
}

void Texture::CreateTextureImage()
{
	ERM_ASSERT(!mPath.empty());

	stbi_set_flip_vertically_on_load(1);
	int width, height, bPP;
	mLocalBuffer = stbi_load(mPath.c_str(), &width, &height, &bPP, STBI_rgb_alpha);

	mWidth = static_cast<uint32_t>(width);
	mHeight = static_cast<uint32_t>(height);
	mBPP = static_cast<uint32_t>(bPP);
	vk::DeviceSize imageSize = mWidth * mHeight * 4;

	ERM_ASSERT(mLocalBuffer);

	mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1;
	mArrayLayers = 1;
	mFormat = vk::Format::eR8G8B8A8Srgb;

	HostBuffer stagingBuffer(mDevice, imageSize, BufferUsage::TRANSFER_SRC);
	stagingBuffer.Update(mLocalBuffer);

	stbi_image_free(mLocalBuffer);

	vk::ImageCreateInfo imageInfo {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = GetWidth();
	imageInfo.extent.height = GetHeight();
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = GetMipLevels();
	imageInfo.arrayLayers = GetArrayLayers();
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
		static_cast<uint32_t>(GetWidth()),
		static_cast<uint32_t>(GetHeight()));
}

void Texture::GenerateMipmaps()
{
	vk::FormatProperties properties = mDevice.GetVkPhysicalDevice().getFormatProperties(GetImageFormat());

	if (!(properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear) || mMipLevels <= 1)
	{
		mMipLevels = 1;
		TransitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
		return;
	}

	vk::CommandBuffer commandBuffer = VkUtils::BeginSingleTimeCommands(mDevice);

	vk::ImageMemoryBarrier barrier {};
	barrier.image = mImage;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	uint32_t mipWidth = mWidth;
	uint32_t mipHeight = mHeight;

	for (uint32_t i = 1; i < mMipLevels; ++i)
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
			mImage, 
			vk::ImageLayout::eTransferSrcOptimal, 
			mImage, 
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

	VkUtils::EndSingleTimeCommands(mDevice, commandBuffer);

	mImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
}

void Texture::CreateTextureImageView()
{
	vk::ImageViewCreateInfo viewInfo {};
	viewInfo.image = GetImage();
	viewInfo.viewType = GetImageViewType();
	viewInfo.format = GetImageFormat();
	viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = GetMipLevels();
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = GetArrayLayers();

	mImageView = VkUtils::CreateImageView(
		mDevice.GetVkDevice(),
		viewInfo);
}

} // namespace erm
