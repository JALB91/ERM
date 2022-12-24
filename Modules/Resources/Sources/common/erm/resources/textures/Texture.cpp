#include "erm/resources/textures/Texture.h"

#include <erm/utils/Utils.h>

#include <stb_image.h>

#include <cmath>

namespace erm {

Texture::Texture(std::string_view path)
	: IAsset(path, path)
	, mBuffer(nullptr)
	, mWidth(0)
	, mHeight(0)
	, mBPP(0)
	, mMipLevels(0)
	, mArrayLayers(1)
{
	ERM_ASSERT(!mPath.empty());

	stbi_set_flip_vertically_on_load(1);
	int width, height, bPP;
	mBuffer = stbi_load(mPath.c_str(), &width, &height, &bPP, STBI_rgb_alpha);
	
	ERM_ASSERT(mBuffer);

	mWidth = static_cast<uint32_t>(width);
	mHeight = static_cast<uint32_t>(height);
	mBPP = static_cast<uint32_t>(bPP);

	mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1;
}

Texture::Texture(
	std::string_view name,
	uint32_t width,
	uint32_t height)
	: IAsset(name, name)
	, mBuffer(nullptr)
	, mWidth(width)
	, mHeight(height)
	, mBPP(0)
	, mMipLevels(1)
	, mArrayLayers(1)
{}

Texture::~Texture()
{
	if (mBuffer)
	{
		stbi_image_free(mBuffer);
		mBuffer = nullptr;
	}
}

} // namespace erm
