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
	
	const TextureType mTextureType = TextureType::CUBE_MAP;
	std::array<unsigned char*, kFaces.size()> mBuffers = {nullptr};
	u32 mWidth, mHeight, mBPP = 0;

};

} // namespace erm
