#pragma once

#include "erm/rendering/data_structs/IAsset.h"

#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

namespace erm {
	class Device;
}

namespace erm {

	class Texture : public IAsset
	{
	public:
		Texture(Device& device, const char* path);
		~Texture();

		Texture(Texture&&) = delete;
		Texture(const Texture&) = delete;

		Texture& operator=(Texture&&) = delete;
		Texture& operator=(const Texture&) = delete;

		inline int GetWidth() const { return mWidth; }
		inline int GetHeight() const { return mHeight; }

		inline vk::ImageView& GetImageView() { return mTextureImageView; }

	private:
		void CreateTextureImage();
		void CreateTextureImageView();

		Device& mDevice;
		unsigned char* mLocalBuffer;
		int mWidth, mHeight, mBPP;

		vk::Image mTextureImage;
		vk::ImageView mTextureImageView;
		vk::DeviceMemory mTextureImageMemory;
	};

} // namespace erm
