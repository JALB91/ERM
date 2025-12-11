#pragma once

#include "erm/rendering/Device.h"

#include <erm/math/Types.h>

#include <vulkan/vulkan.hpp>

namespace erm {
struct CubeMap;
struct Texture;
}

namespace erm {

template<typename T>
class GPUImage
{
private:
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

public:
	GPUImage(Device& device, T texture);
	GPUImage(
		Device& device,
		T texture,
		u32 mipLevels,
		u32 arrayLayers,
		vk::Image image,
		vk::ImageView imageView,
		vk::DeviceMemory imageMemory,
		vk::ImageLayout imageLayout,
		vk::Format format,
		bool ownsImage)
		: mDevice(device)
		, mTexture(std::move(texture))
		, mMipLevels(mipLevels)
		, mArrayLayers(arrayLayers)
		, mImage(image)
		, mImageView(imageView)
		, mImageMemory(imageMemory)
		, mImageLayout(imageLayout)
		, mFormat(format)
		, mImageViewType(ToImageViewType<T>())
		, mOwnsImage(ownsImage)
	{}
	
	~GPUImage()
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
	
	GPUImage(GPUImage&&) = delete;
	GPUImage(const GPUImage&) = delete;

	GPUImage& operator=(GPUImage&&) = delete;
	GPUImage& operator=(const GPUImage&) = delete;
	
	inline vk::Image getImage() const { return mImage; }
	inline vk::DeviceMemory getImageMemory() const { return mImageMemory; }
	inline vk::Format getImageFormat() const { return mFormat; }
	inline vk::ImageViewType getImageViewType() const { return mImageViewType; }
	inline vk::ImageView getImageView() const { return mImageView; }
	inline vk::ImageLayout getImageLayout() const { return mImageLayout; }
	
	inline void setOwnsImage(bool ownsImage) { mOwnsImage = ownsImage; }

private:
	void createTextureImage();
	void transitionImageLayout(vk::ImageLayout newLayout);
	void generateMipmaps();
	void createTextureImageView();

	Device& mDevice;
	T mTexture;
	
	u32 mMipLevels, mArrayLayers;
	
	vk::Image mImage;
	vk::ImageView mImageView;
	vk::DeviceMemory mImageMemory;
	vk::ImageLayout mImageLayout;
	vk::Format mFormat;
	vk::ImageViewType mImageViewType;
	bool mOwnsImage;
};

} // namespace erm
