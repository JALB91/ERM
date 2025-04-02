#pragma once

#include "erm/nn/IStatementHandler.h"
#include "erm/nn/Token.h"

#include <erm/math/Types.h>

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace erm::nn {

class Program
{
public:
	Program(std::string_view name) noexcept;
	
	Program(const Program&) noexcept = delete;
	Program& operator=(const Program&) noexcept = delete;
	
	Program(Program&& other) = delete;
	Program& operator=(Program&& other) = delete;
	
	inline const std::unordered_map<std::string, std::unique_ptr<IStatementHandler>>& getStatementHandlers() const
	{
		return mStatementHandlers;
	}
	
	inline const std::vector<Token>& getTokens() const { return mTokens; }
	
	inline const std::string& getName() const { return mName; }
	
	bool parse(std::string_view text);
	void debugPrint() const;

private:
	std::unordered_map<std::string, std::unique_ptr<IStatementHandler>> mStatementHandlers;
	std::vector<Token> mTokens;
	std::string mName;

};

}
