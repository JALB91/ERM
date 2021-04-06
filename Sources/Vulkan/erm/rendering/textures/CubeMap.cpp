#include "erm/rendering/textures/CubeMap.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/HostBuffer.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <stb_image.h>

#include <array>
#include <filesystem>

namespace {

const std::array kFaces {
	"right",
	"left",
	"top",
	"bottom",
	"front",
	"back"};

}

namespace erm {

CubeMap::CubeMap(Device& device, const char* path)
	: Texture(device, path)
{}

CubeMap::~CubeMap() = default;

void CubeMap::CreateTextureImage()
{
	stbi_set_flip_vertically_on_load(0);

	std::string targetExt;

	for (const auto& entry : std::filesystem::directory_iterator(mPath))
	{
		if (entry.path().has_extension())
		{
			targetExt = entry.path().extension().string();
			break;
		}
	}

	std::unique_ptr<HostBuffer> stagingBuffer;

	for (size_t i = 0; i < kFaces.size(); ++i)
	{
		mLocalBuffer = stbi_load((mPath + "/" + kFaces[i] + targetExt).c_str(), &mWidth, &mHeight, &mBPP, STBI_rgb_alpha);
		vk::DeviceSize imageSize = mWidth * mHeight * 4;
		ASSERT(mLocalBuffer);

		if (!stagingBuffer)
			stagingBuffer = std::make_unique<HostBuffer>(mDevice, imageSize * kFaces.size(), vk::BufferUsageFlagBits::eTransferSrc);
		stagingBuffer->Update(mLocalBuffer, BufferInfo(i * imageSize, imageSize));

		stbi_image_free(mLocalBuffer);
	}

	vk::ImageCreateInfo imageInfo {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = mWidth;
	imageInfo.extent.height = mHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 6;
	imageInfo.format = vk::Format::eR8G8B8A8Srgb;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;
	imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;

	VkUtils::CreateImageUnique(
		mDevice.GetVkPhysicalDevice(),
		mDevice.GetVkDevice(),
		imageInfo,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		mTextureImage,
		mTextureImageMemory);

	VkUtils::TransitionImageLayout(
		mDevice,
		mTextureImage.get(),
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal,
		6);

	VkUtils::CopyBufferToImage(
		mDevice,
		stagingBuffer->GetBuffer(),
		mTextureImage.get(),
		static_cast<uint32_t>(mWidth),
		static_cast<uint32_t>(mHeight),
		6);

	VkUtils::TransitionImageLayout(
		mDevice,
		mTextureImage.get(),
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageLayout::eTransferDstOptimal,
		vk::ImageLayout::eShaderReadOnlyOptimal,
		6);
}

void CubeMap::CreateTextureImageView()
{
	vk::ImageViewCreateInfo viewInfo {};
	viewInfo.image = mTextureImage.get();
	viewInfo.viewType = vk::ImageViewType::eCube;
	viewInfo.format = vk::Format::eR8G8B8A8Srgb;
	viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 6;

	mTextureImageView = VkUtils::CreateImageViewUnique(
		mDevice.GetVkDevice(),
		viewInfo);
}

} // namespace erm
