#pragma once

#include "erm/nn/TokenType.h"

#include <erm/utils/StaticString.h>

namespace erm::nn {

struct Token 
{
	str128 mValue;
    TokenType mType = TokenType::INVALID;
};

}