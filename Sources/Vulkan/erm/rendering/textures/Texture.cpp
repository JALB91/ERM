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
	, mImage(nullptr)
	, mImageView(nullptr)
	, mImageMemory(nullptr)
	, mImageLayout(vk::ImageLayout::eGeneral)
	, mFormat(vk::Format::eR8G8B8A8Srgb)
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

	HostBuffer stagingBuffer(mDevice, imageSize, BufferUsage::TRANSFER_SRC);
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
		mImage,
		mImageMemory,
		MemoryProperty::DEVICE_LOCAL);

	VkUtils::TransitionImageLayout(
		mDevice,
		mImage,
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal);

	VkUtils::CopyBufferToImage(
		mDevice,
		stagingBuffer.GetBuffer(),
		mImage,
		static_cast<uint32_t>(mWidth),
		static_cast<uint32_t>(mHeight));

	VkUtils::TransitionImageLayout(
		mDevice,
		mImage,
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageLayout::eTransferDstOptimal,
		vk::ImageLayout::eShaderReadOnlyOptimal);

	mImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	mFormat = vk::Format::eR8G8B8A8Srgb;
}

void Texture::CreateTextureImageView()
{
	vk::ImageViewCreateInfo viewInfo {};
	viewInfo.image = mImage;
	viewInfo.viewType = vk::ImageViewType::e2D;
	viewInfo.format = vk::Format::eR8G8B8A8Srgb;
	viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	mImageView = VkUtils::CreateImageView(
		mDevice.GetVkDevice(),
		viewInfo);
}

} // namespace erm
