#pragma once

#include "erm/nn/Tokenizer.h"
#include "erm/nn/Program.h"

#include <optional>
#include <string_view>

namespace erm::nn {

class Parser
{
public:
	Parser() noexcept = default;

	bool parseStatements(std::string_view str);
	std::optional<Program> parseProgram(std::string_view name, std::string_view str);

private:
	Tokenizer mTokenizer;

};

}
