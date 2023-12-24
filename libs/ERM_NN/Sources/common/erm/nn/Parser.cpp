#include "erm/nn/Parser.h"

#include "erm/nn/Tokenizer.h"

#include <erm/utils/assert/Assert.h>
#include <erm/utils/Utils.h>

namespace erm::nn {

bool Parser::parseStatements(std::string_view str)
{
    str128 statement;
	for (const auto ch : str)
    {
		if (
			isalpha(ch) != 0 ||
			(isdigit(ch) != 0 && !statement.empty()))
		{
			statement += ch;
		}
		else if (ch == '\n')
		{
			if (!statement.empty())
			{
				mTokenizer.addStatement(statement);
				statement.clear();
			}
		}
		else
		{
			ERM_LOG_ERROR("Invalid character detected while parsing statements");
			return false;
		}
    }
	
	return true;
}

std::optional<Program> Parser::parseProgram(std::string_view name, std::string_view str)
{
    mTokenizer.init(str);

    auto lookahead = mTokenizer.getNextToken();

    Program prog(name);

    while (lookahead.has_value())
    {
		if (lookahead->mType == TokenType::INVALID || !prog.addToken(lookahead.value()))
		{
			ERM_LOG_ERROR(
				"Invalid token \"%s\", while parsing \"%s\" at line %d, %d",
				lookahead->mValue.data(),
				name.data(),
				lookahead->mLine,
				lookahead->mLineOffset);
			return std::nullopt;
		}
		
		lookahead = mTokenizer.getNextToken();
    }

    return prog;
}

}
