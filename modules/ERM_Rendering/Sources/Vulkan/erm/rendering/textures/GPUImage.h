#pragma once

#include <erm/math/Types.h>

#include <vulkan/vulkan.hpp>

namespace erm {
class Device;
struct CubeMap;
struct Texture;
}

namespace erm {

template<typename T>
class GPUImage
{
public:
	GPUImage(Device& device, T& texture);
	GPUImage(
		Device& device,
		T& texture,
		u32 mipLevels,
		u32 arrayLayers,
		vk::Image image,
		vk::ImageView imageView,
		vk::DeviceMemory imageMemory,
		vk::ImageLayout imageLayout,
		vk::Format format);
	~GPUImage();
	
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
	const T& mTexture;
	
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
