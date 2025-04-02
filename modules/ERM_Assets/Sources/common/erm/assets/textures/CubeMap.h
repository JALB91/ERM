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
	
	CubeMap()
		: mTextureType(TextureType::CUBE_MAP)
		, mBuffers{nullptr}
		, mWidth(0)
		, mHeight(0)
		, mBPP(0)
	{}
	
	TextureType mTextureType;
	std::array<unsigned char*, kFaces.size()> mBuffers;
	u32 mWidth, mHeight, mBPP;

};

} // namespace erm
