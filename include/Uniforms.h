#pragma once

#include "Utils.h"

#include <array>
#include <string>

namespace erm {
	
	enum class Uniforms
	{
		MVP = 			0,
		TEXTURE_2D = 	1,
		COUNT
	};
	
	static std::string const GetUniformName(const Uniforms& uniform)
	{
		switch (uniform)
		{
			case Uniforms::MVP: 		return "u_MVP";
			case Uniforms::TEXTURE_2D: 	return "u_Texture";
			default:
				ASSERT(false);
				return "";
		}
	}
	
	static const std::array<Uniforms, static_cast<int>(Uniforms::COUNT)> kUniforms
	{
		Uniforms::MVP,
		Uniforms::TEXTURE_2D
	};
	
}
