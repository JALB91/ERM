#pragma once

#include "erm/assets/enums/TextureType.h"

#include <erm/math/Types.h>

namespace erm {

struct Texture
{
	u8* mBuffer = nullptr;
	u32 mWidth, mHeight, mBPP = 0;
	TextureType mTextureType = TextureType::COUNT;
};

} // namespace erm
