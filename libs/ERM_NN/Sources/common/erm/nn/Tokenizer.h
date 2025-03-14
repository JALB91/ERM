#pragma once

#include "erm/nn/Cursor.h"
#include "erm/nn/Token.h"

#include <optional>
#include <string_view>

namespace erm::nn {

class Tokenizer
{
public:
	Tokenizer(std::string_view text) noexcept;

	std::optional<Token> getNextToken();
	
	inline const Cursor& getCursor() const
	{
		return mCursor;
	}

private:
	void updateCursor();

	Cursor mCursor;

};

}
