#pragma once

#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

namespace erm {
	class Device;
}

namespace erm {

	class Texture
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

		inline const std::string& GetPath() const { return mPath; }

		inline vk::ImageView& GetImageView() { return mTextureImageView; }

	private:
		void CreateTextureImage();
		void CreateTextureImageView();

		Device& mDevice;
		unsigned char* mLocalBuffer;
		int mWidth, mHeight, mBPP;
		const std::string mPath;

		vk::Image mTextureImage;
		vk::ImageView mTextureImageView;
		vk::DeviceMemory mTextureImageMemory;
	};

} // namespace erm
