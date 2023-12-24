#pragma once

#include "erm/nn/Cursor.h"
#include "erm/nn/Token.h"

#include <erm/math/Types.h>

#include <erm/utils/StaticString.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace erm::nn {

class Tokenizer
{
public:
	Tokenizer() noexcept;

	void addStatement(std::string_view statement);
	void init(std::string_view text);

	std::optional<Token> getNextToken();
	
	inline const Cursor& getCursor() const
	{
		return mCursor;
	}

private:
	void updateCursor();

	std::vector<str128> mStatements;
	Cursor mCursor;

};

}
