#pragma once

namespace erm::nn {

enum class TokenType
{
	INVALID = 0,
	NUMERIC_LITERAL,
	STRING_LITERAL,
	IDENTIFIER,
	SYMBOL
};

}
