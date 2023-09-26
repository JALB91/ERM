#include "erm/nn/Tokenizer.h"

#include <erm/utils/Utils.h>

#include <algorithm>

namespace erm::nn {

Tokenizer::Tokenizer() noexcept
	: mStatements {
		"enum",
		"class",
		"struct",
		"import",
		"int",
		"uint",
		"float",
		"double",
		"long"
	}
	, mCursor(0)
{}

void Tokenizer::Init(std::string_view str)
{
	mString = str;
	mCursor = 0;
}

void Tokenizer::addStatement(std::string_view statement)
{
	const auto it = std::find(mStatements.begin(), mStatements.end(), statement);
	if (it != mStatements.end())
	{
		ERM_ASSERT(false);
		return;
	}

	mStatements.emplace_back(statement);
}

void Tokenizer::UpdateCursor()
{
	while (HasMoreTokens() && (mString[mCursor] == ' ' || mString[mCursor] == '\n'))
	{
		++mCursor;
	}

	if (mString[mCursor] == '/' && mString.size() > mCursor + 1 && mString[mCursor + 1] == '/')
	{
		while (HasMoreTokens() && mString[mCursor] != '\n')
		{
			++mCursor;
		}

		++mCursor;
		UpdateCursor();
	}

	if (mString[mCursor] == '/' && mString.size() > mCursor + 1 && mString[mCursor + 1] == '*')
	{
		mCursor += 2;

		while (HasMoreTokens() && mString[mCursor] != '*' && mString.size() > mCursor + 1 && mString[mCursor + 1] != '/')
		{
			++mCursor;
		}

		mCursor += 2;
		UpdateCursor();
	}
}

std::optional<Token> Tokenizer::GetNextToken()
{
	UpdateCursor();

	if (!HasMoreTokens())
	{
		return std::nullopt;
	}

	Token token;

	if (isdigit(mString[mCursor]) != 0)
	{
		token.mType = TokenType::NUMERIC_LITERAL;
		while (isdigit(mString[mCursor]) != 0)
		{
			token.mValue += mString[mCursor++];
		}
	}
	else if (mString[mCursor] == '"')
	{
		token.mType = TokenType::STRING_LITERAL;
		while (mString[++mCursor] != '"')
		{
			token.mValue += mString[mCursor];
		}
		++mCursor;
	}
	else if (mString[mCursor] == ';')
	{
		token.mType = TokenType::COLUMN;
		token.mValue = mString[mCursor++];
	}
	else if (mString[mCursor] == ',')
	{
		token.mType = TokenType::COMMA;
		token.mValue = mString[mCursor++];
	}
	else if (mString[mCursor] == '{' || mString[mCursor] == '[' || mString[mCursor] == '(' || mString[mCursor] == '<')
	{
		token.mType = TokenType::SCOPE_BEGIN;
		token.mValue = mString[mCursor++];
	}
	else if (mString[mCursor] == '}' || mString[mCursor] == ']' || mString[mCursor] == ')' || mString[mCursor] == '>')
	{
		token.mType = TokenType::SCOPE_END;
		token.mValue = mString[mCursor++];
	}
	else
	{
		const auto statementIt = std::find_if(mStatements.begin(), mStatements.end(), [&](const auto& statement) {
			return mString.size() - mCursor > statement.size() && mString.substr(mCursor, statement.size()) == statement;
		});

		if (statementIt != mStatements.end())
		{
			const auto& statement = *statementIt;
			token.mType = TokenType::STATEMENT;
			token.mValue = statement;
			mCursor += statement.size();
		}
		else if (mString[mCursor] == '=')
		{
			token.mType = TokenType::OPERATOR;
			token.mValue = mString[mCursor++];
		}
		else if (isalpha(mString[mCursor]) != 0)
		{
			token.mType = TokenType::IDENTIFIER;
			while (isalpha(mString[mCursor]) != 0)
			{
				token.mValue += mString[mCursor++];
			}
		}
	}

	return token;
}

bool Tokenizer::HasMoreTokens() const
{
	return mCursor < mString.size();
}

}
