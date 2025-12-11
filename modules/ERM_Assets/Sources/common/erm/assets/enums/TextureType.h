#pragma once

#include <erm/math/Types.h>

namespace erm {

enum class TextureType : u8
{
	DIFFUSE = 0,
	NORMAL,
	SPECULAR,
	CUBE_MAP,
	ALBEDO,
	METALLIC,
	ROUGHNESS,
	COUNT
};

}
