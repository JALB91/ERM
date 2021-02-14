#include "erm/rendering/textures/Texture.h"

#include "erm/rendering/Device.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <stb_image.h>

#include <fstream>
#include <iostream>

namespace erm {

	Texture::Texture(Device& device, const char* path)
		: IAsset(path, "")
		, mDevice(device)
		, mLocalBuffer(nullptr)
		, mWidth(0)
		, mHeight(0)
		, mBPP(0)
	{
		CreateTextureImage();
		CreateTextureImageView();
	}

	Texture::~Texture()
	{
		mDevice->freeMemory(mTextureImageMemory);
		mDevice->destroyImageView(mTextureImageView);
		mDevice->destroyImage(mTextureImage);
	}

	void Texture::CreateTextureImage()
	{
		stbi_set_flip_vertically_on_load(1);
		mLocalBuffer = stbi_load(mPath.c_str(), &mWidth, &mHeight, &mBPP, STBI_rgb_alpha);
		vk::DeviceSize imageSize = mWidth * mHeight * 4;

		if (!mLocalBuffer)
		{
			throw std::runtime_error("Failed to load texture image");
		}

		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;

		VkUtils::CreateBuffer(
			mDevice.GetVkPhysicalDevice(),
			mDevice.GetVkDevice(),
			imageSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBuffer,
			stagingBufferMemory);

		void* data = mDevice->mapMemory(stagingBufferMemory, 0, imageSize);
		memcpy(data, mLocalBuffer, static_cast<size_t>(imageSize));
		mDevice->unmapMemory(stagingBufferMemory);

		stbi_image_free(mLocalBuffer);

		VkUtils::CreateImage(
			mDevice.GetVkPhysicalDevice(),
			mDevice.GetVkDevice(),
			mWidth,
			mHeight,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			mTextureImage,
			mTextureImageMemory);

		VkUtils::TransitionImageLayout(
			mDevice.GetGraphicsQueue(),
			mDevice.GetCommandPool(),
			mDevice.GetVkDevice(),
			mTextureImage,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eTransferDstOptimal);

		VkUtils::CopyBufferToImage(
			mDevice.GetGraphicsQueue(),
			mDevice.GetCommandPool(),
			mDevice.GetVkDevice(),
			stagingBuffer,
			mTextureImage,
			static_cast<uint32_t>(mWidth),
			static_cast<uint32_t>(mHeight));

		VkUtils::TransitionImageLayout(
			mDevice.GetGraphicsQueue(),
			mDevice.GetCommandPool(),
			mDevice.GetVkDevice(),
			mTextureImage,
			vk::Format::eR8G8B8A8Srgb,
			vk::ImageLayout::eTransferDstOptimal,
			vk::ImageLayout::eShaderReadOnlyOptimal);

		mDevice->destroyBuffer(stagingBuffer);
		mDevice->freeMemory(stagingBufferMemory);
	}

	void Texture::CreateTextureImageView()
	{
		mTextureImageView = VkUtils::CreateImageView(mDevice.GetVkDevice(), mTextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
	}

} // namespace erm
