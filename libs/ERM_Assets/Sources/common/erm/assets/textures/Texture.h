#pragma once

#include "erm/assets/enums/TextureType.h"

#include <erm/math/Types.h>

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
	u32 mWidth, mHeight, mBPP;

};

} // namespace erm
