#pragma once

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

	void Init(std::string_view str);
	void addStatement(std::string_view statement);

	std::optional<Token> GetNextToken();

private:
	void UpdateCursor();
	bool HasMoreTokens() const;

	std::vector<str128> mStatements;
	std::string mString;
	u64 mCursor;

};

}