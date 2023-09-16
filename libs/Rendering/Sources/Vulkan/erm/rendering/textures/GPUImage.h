#pragma once

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
		uint32_t mipLevels,
		uint32_t arrayLayers,
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
	
	inline vk::Image GetImage() const { return mImage; }
	inline vk::DeviceMemory GetImageMemory() const { return mImageMemory; }
	inline vk::Format GetImageFormat() const { return mFormat; }
	inline vk::ImageViewType GetImageViewType() const { return mImageViewType; }
	inline vk::ImageView GetImageView() const { return mImageView; }
	inline vk::ImageLayout GetImageLayout() const { return mImageLayout; }
	
	inline void SetOwnsImage(bool ownsImage) { mOwnsImage = ownsImage; }

private:
	void CreateTextureImage();
	void TransitionImageLayout(vk::ImageLayout newLayout);
	void GenerateMipmaps();
	void CreateTextureImageView();

	Device& mDevice;
	const T& mTexture;
	
	uint32_t mMipLevels, mArrayLayers;
	
	vk::Image mImage;
	vk::ImageView mImageView;
	vk::DeviceMemory mImageMemory;
	vk::ImageLayout mImageLayout;
	vk::Format mFormat;
	vk::ImageViewType mImageViewType;
	bool mOwnsImage;
};

} // namespace erm
