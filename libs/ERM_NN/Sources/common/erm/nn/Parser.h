#pragma once

#include "erm/nn/Tokenizer.h"
#include "erm/nn/Program.h"

#include <string_view>

namespace erm::nn {

class Parser
{
public:
	Parser() noexcept = default;

	void parseStatements(std::string_view str);
	Program parseProgram(std::string_view name, std::string_view str);

private:
	Tokenizer mTokenizer;

};

}