#pragma once

#include "erm/nn/Token.h"

#include <erm/math/Types.h>

#include <erm/utils/StaticString.h>
#include <erm/utils/Tree.h>

#include <nlohmann/json.hpp>

#include <string_view>

namespace erm::nn {

class Program
{
public:
	Program(std::string_view name) noexcept;

	nlohmann::json toJson() const;

	void addToken(const Token& token);

	inline const str64& getName() const { return mName; }

private:
	using TokenTree = Tree<u64, Token>;
    TokenTree mTokenTree;
	TokenTree* mIndex;
	u64 mCounter;
	str64 mName;

};

}