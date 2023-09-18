#pragma once

namespace erm::nn {

enum class TokenType
{
	INVALID = 0,
	PROGRAM,
	NUMERIC_LITERAL,
	STRING_LITERAL,
	COLUMN,
	IDENTIFIER,
	STATEMENT,
	COMMA,
	SCOPE_BEGIN,
	SCOPE_END,
	OPERATOR
};

}