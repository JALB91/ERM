#include "erm/nn/Parser.h"

#include "erm/nn/Tokenizer.h"

#include <erm/utils/Utils.h>

namespace erm::nn {

void Parser::parseStatements(std::string_view str)
{
    str128 statement;
	for (const auto ch : str)
    {
        if (ch == ' ')
        {
            continue;
        }
        else if (ch == '\n' && !statement.empty())
        {
            mTokenizer.addStatement(statement);
            statement.clear();
            continue;
        }
        else if (!isalpha(ch) && !isdigit(ch))
        {
            ERM_ASSERT_HARD(false);
            return;
        }
        else
        {
            statement += ch;
        }
    }
}

Program Parser::parseProgram(std::string_view name, std::string_view str)
{
    mTokenizer.Init(str);

    auto lookahead = mTokenizer.GetNextToken();

    Program prog(name);

    while (lookahead.has_value())
    {
		ERM_ASSERT(lookahead.value().mType != TokenType::INVALID);

		prog.addToken(lookahead.value());
		lookahead = mTokenizer.GetNextToken();
    }

    return prog;
}

}