#pragma once

#include "erm/nn/TokenType.h"

namespace erm::nn {

struct Token 
{
	std::string mValue;
	TokenType mType = TokenType::INVALID;
	u16 mLine = 0;
	u16 mLineOffset = 0;
};

}
