#include "erm/rendering/textures/Texture.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/HostBuffer.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <stb_image.h>

namespace erm {

Texture::Texture(
	Device& device,
	const char* path,
	vk::Image image /* = nullptr*/,
	vk::ImageView imageView /* = nullptr*/,
	vk::DeviceMemory imageMemory /* = nullptr*/)
	: IAsset(path, "")
	, mDevice(device)
	, mLocalBuffer(nullptr)
	, mWidth(0)
	, mHeight(0)
	, mBPP(0)
	, mTextureImage(image)
	, mTextureImageView(imageView)
	, mTextureImageMemory(imageMemory)
	, mImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
{}

Texture::~Texture()
{
	mDevice->destroyImage(mTextureImage);
	mDevice->destroyImageView(mTextureImageView);
	mDevice->freeMemory(mTextureImageMemory);
}

void Texture::Init()
{
	ERM_ASSERT(!mTextureImage && !mTextureImageView && !mTextureImageMemory);
	CreateTextureImage();
	CreateTextureImageView();
}

void Texture::CreateTextureImage()
{
	stbi_set_flip_vertically_on_load(1);
	mLocalBuffer = stbi_load(mPath.c_str(), &mWidth, &mHeight, &mBPP, STBI_rgb_alpha);
	vk::DeviceSize imageSize = mWidth * mHeight * 4;
	ERM_ASSERT(mLocalBuffer);

	HostBuffer stagingBuffer(mDevice, imageSize, vk::BufferUsageFlagBits::eTransferSrc);
	stagingBuffer.Update(mLocalBuffer);

	stbi_image_free(mLocalBuffer);

	vk::ImageCreateInfo imageInfo {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = mWidth;
	imageInfo.extent.height = mHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = vk::Format::eR8G8B8A8Srgb;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;

	VkUtils::CreateImage(
		mDevice.GetVkPhysicalDevice(),
		mDevice.GetVkDevice(),
		imageInfo,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		mTextureImage,
		mTextureImageMemory);

	VkUtils::TransitionImageLayout(
		mDevice,
		mTextureImage,
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal);

	VkUtils::CopyBufferToImage(
		mDevice,
		stagingBuffer.GetBuffer(),
		mTextureImage,
		static_cast<uint32_t>(mWidth),
		static_cast<uint32_t>(mHeight));

	VkUtils::TransitionImageLayout(
		mDevice,
		mTextureImage,
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageLayout::eTransferDstOptimal,
		vk::ImageLayout::eShaderReadOnlyOptimal);
}

void Texture::CreateTextureImageView()
{
	vk::ImageViewCreateInfo viewInfo {};
	viewInfo.image = mTextureImage;
	viewInfo.viewType = vk::ImageViewType::e2D;
	viewInfo.format = vk::Format::eR8G8B8A8Srgb;
	viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	mTextureImageView = VkUtils::CreateImageView(
		mDevice.GetVkDevice(),
		viewInfo);
}

} // namespace erm
