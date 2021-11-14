#pragma once

#include "erm/rendering/data_structs/IAsset.h"

#include <vulkan/vulkan.hpp>

namespace erm {
class Device;
}

namespace erm {

class Texture : public IAsset
{
public:
	Texture(
		Device& device,
		const char* path,
		vk::Image image = nullptr,
		vk::ImageView imageView = nullptr,
		vk::DeviceMemory deviceMemory = nullptr);
	virtual ~Texture();

	void Init();

	Texture(Texture&&) = delete;
	Texture(const Texture&) = delete;

	Texture& operator=(Texture&&) = delete;
	Texture& operator=(const Texture&) = delete;

	inline void SetImageLayout(vk::ImageLayout imageLayout) { mImageLayout = imageLayout; }

	inline int GetWidth() const { return mWidth; }
	inline int GetHeight() const { return mHeight; }

	inline vk::Image& GetImage() { return mTextureImage; }
	inline vk::ImageView& GetImageView() { return mTextureImageView; }
	inline vk::DeviceMemory& GetImageMemory() { return mTextureImageMemory; }
	inline vk::ImageLayout GetImageLayout() const { return mImageLayout; }

protected:
	virtual void CreateTextureImage();
	virtual void CreateTextureImageView();

	Device& mDevice;
	unsigned char* mLocalBuffer;
	int mWidth, mHeight, mBPP;

	vk::Image mTextureImage;
	vk::ImageView mTextureImageView;
	vk::DeviceMemory mTextureImageMemory;
	vk::ImageLayout mImageLayout;
};

} // namespace erm
