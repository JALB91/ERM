#pragma once

#include "erm/nn/Token.h"

#include <erm/math/Types.h>

#include <erm/utils/ContainerItemHandle.h>
#include <erm/utils/StaticString.h>
#include <erm/utils/Tree.h>

#include <nlohmann/json.hpp>

#include <string_view>
#include <vector>

namespace erm::nn {

class Program
{
public:
	Program(std::string_view name) noexcept;

	bool addToken(const Token& token);
	
	bool validateTree() const;
	nlohmann::json toJson() const;

	inline const str64& getName() const { return mName; }

private:
	using TokenTree = Tree<u64, VectorItemHandle<Token>>;
	std::vector<Token> mTokens;
	TokenTree mTokenTree;
	TokenTree* mIndex;
	str64 mName;

};

}
