#include "erm/assets/textures/Texture.h"

#include <erm/utils/Utils.h>

#include <stb_image.h>

#include <cmath>

namespace erm {

//Texture::Texture(std::string_view name)
//	: IResource(name, TYPE)
//	, mBuffer(nullptr)
//	, mWidth(0)
//	, mHeight(0)
//	, mBPP(0)
//	, mMipLevels(0)
//	, mArrayLayers(1)
//{
////	ERM_ASSERT(!mPath.empty());
////
////	stbi_set_flip_vertically_on_load(1);
////
////	int width, height, bPP;
////	mBuffer = stbi_load(mPath.c_str(), &width, &height, &bPP, STBI_rgb_alpha);
////
////	ERM_ASSERT(mBuffer);
////
////	mWidth = static_cast<u32>(width);
////	mHeight = static_cast<u32>(height);
////	mBPP = static_cast<u32>(bPP);
////
////	mMipLevels = static_cast<u32>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1;
//}
//
//Texture::Texture(
//	std::string_view name,
//	u32 width,
//	u32 height)
//	: IResource(name, TYPE)
//	, mBuffer(nullptr)
//	, mWidth(width)
//	, mHeight(height)
//	, mBPP(0)
//	, mMipLevels(1)
//	, mArrayLayers(1)
//{}
//
//Texture::~Texture()
//{
//	if (mBuffer)
//	{
//		stbi_image_free(mBuffer);
//		mBuffer = nullptr;
//	}
//}

} // namespace erm
