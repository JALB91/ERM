#pragma once

#include "erm/rendering/data_structs/IAsset.h"

#include <vulkan/vulkan.hpp>

namespace erm {
class Device;
class IRenderer;
}

namespace erm {

class Texture : public IAsset
{
public:
	friend class IRenderer;

public:
	Texture(Device& device);
	virtual ~Texture();

	void InitFromFile(const char* path);
	void InitWithData(
		const char* name,
		vk::Image image,
		vk::ImageView imageView,
		vk::DeviceMemory deviceMemory,
		vk::ImageLayout imageLayout,
		vk::Format format,
		uint32_t width,
		uint32_t height
	);

	Texture(Texture&&) = delete;
	Texture(const Texture&) = delete;

	Texture& operator=(Texture&&) = delete;
	Texture& operator=(const Texture&) = delete;

	inline int GetWidth() const { return mWidth; }
	inline int GetHeight() const { return mHeight; }
	inline const vk::Image& GetImage() const { return mImage; }
	inline const vk::ImageView& GetImageView() const { return mImageView; }
	inline const vk::DeviceMemory& GetImageMemory() const { return mImageMemory; }
	inline vk::ImageLayout GetImageLayout() const { return mImageLayout; }
	inline vk::Format GetImageFormat() const { return mFormat; }

protected:
	virtual void CreateTextureImage();
	virtual void CreateTextureImageView();

	Device& mDevice;
	unsigned char* mLocalBuffer;
	uint32_t mWidth, mHeight, mBPP;

	vk::Image mImage;
	vk::ImageView mImageView;
	vk::DeviceMemory mImageMemory;
	vk::ImageLayout mImageLayout;
	vk::Format mFormat;
};

} // namespace erm
