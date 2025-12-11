#pragma once

#include "erm/assets/enums/TextureType.h"

#include <erm/math/Types.h>

#include <array>

namespace erm {

struct CubeMap
{
	static constexpr std::array kFaces {
		"right",
		"left",
		"top",
		"bottom",
		"front",
		"back"};
	
	std::array<u8*, kFaces.size()> mBuffers = {nullptr};
	u32 mWidth, mHeight, mBPP = 0;
	const TextureType mTextureType = TextureType::CUBE_MAP;
};

} // namespace erm
