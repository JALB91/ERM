#pragma once

#include "erm/assets/enums/TextureType.h"

#include <erm/math/Types.h>

namespace erm {

struct Texture
{
	TextureType mTextureType = TextureType::COUNT;
	unsigned char* mBuffer = nullptr;
	u32 mWidth, mHeight, mBPP = 0;

};

} // namespace erm
