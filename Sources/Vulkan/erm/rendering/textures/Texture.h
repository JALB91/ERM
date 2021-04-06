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
	Texture(Device& device, const char* path);
	virtual ~Texture();

	void Init();

	Texture(Texture&&) = delete;
	Texture(const Texture&) = delete;

	Texture& operator=(Texture&&) = delete;
	Texture& operator=(const Texture&) = delete;

	inline int GetWidth() const { return mWidth; }
	inline int GetHeight() const { return mHeight; }

	inline vk::ImageView& GetImageView() { return mTextureImageView.get(); }

protected:
	virtual void CreateTextureImage();
	virtual void CreateTextureImageView();

	Device& mDevice;
	unsigned char* mLocalBuffer;
	int mWidth, mHeight, mBPP;

	vk::UniqueImage mTextureImage;
	vk::UniqueImageView mTextureImageView;
	vk::UniqueDeviceMemory mTextureImageMemory;
};

} // namespace erm
