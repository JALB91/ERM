#pragma once

#include "erm/assets/enums/TextureType.h"

#include <cstdint>

namespace erm {

struct Texture
{
	Texture()
		: mTextureType(TextureType::COUNT)
		, mBuffer(nullptr)
		, mWidth(0)
		, mHeight(0)
		, mBPP(0)
	{}
	
	TextureType mTextureType;
	unsigned char* mBuffer;
	uint32_t mWidth, mHeight, mBPP;

};

} // namespace erm
