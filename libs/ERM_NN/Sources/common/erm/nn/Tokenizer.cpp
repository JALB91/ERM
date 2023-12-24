#include "erm/nn/Tokenizer.h"

#include "erm/nn/Common.h"

#include <erm/utils/assert/Assert.h>
#include <erm/utils/Utils.h>

#include <algorithm>

namespace erm::nn {

Tokenizer::Tokenizer() noexcept
	: mStatements {
		kStatements.begin(),
		kStatements.end()
	}
{}

void Tokenizer::addStatement(std::string_view statement)
{
	const auto it = std::find(mStatements.begin(), mStatements.end(), statement);
	if (it != mStatements.end())
	{
		return;
	}
	mStatements.emplace_back(statement);
}

void Tokenizer::init(std::string_view text)
{
	mCursor.init(text);
}

void Tokenizer::updateCursor()
{
	while (mCursor.isSpaceOrTab() || mCursor.isNewLine())
	{
		if (!mCursor.tryAdvance())
		{
			return;
		}
	}

	if (mCursor.isOneLineComment())
	{
		while (!mCursor.isNewLine())
		{
			if (!mCursor.tryAdvance())
			{
				return;
			}
		}
		updateCursor();
		return;
	}

	if (mCursor.isMultilineCommentBegin())
	{
		mCursor.tryAdvance(2);

		while (!mCursor.isMultilineCommentEnd())
		{
			if (!mCursor.tryAdvance())
			{
				return;
			}
		}

		mCursor.tryAdvance(2);
		updateCursor();
		return;
	}
}

std::optional<Token> Tokenizer::getNextToken()
{
	updateCursor();

	if (!mCursor.isValid())
	{
		return std::nullopt;
	}

	Token token;
	token.mLine = mCursor.getCursorLine() + 1;
	token.mLineOffset = mCursor.getCursorLineOffset() + 1;

	if (mCursor.isDigit())
	{
		token.mType = TokenType::NUMERIC_LITERAL;
		do
		{
			token.mValue += *mCursor.getCharAtCursor();
			mCursor.tryAdvance();
		}
		while (mCursor.isDigit());
	}
	else if (mCursor.is('"'))
	{
		do
		{
			if (!mCursor.tryAdvance())
			{
				ERM_LOG_ERROR("Unexpected EOF while parsing string literal");
				return token;
			}
			
			if (!mCursor.is('"'))
			{
				token.mValue += *mCursor.getCharAtCursor();
			}
		}
		while (!mCursor.is('"'));
		
		token.mType = TokenType::STRING_LITERAL;
		mCursor.tryAdvance();
	}
	else if (mCursor.is(';'))
	{
		token.mType = TokenType::SEMI_COLON;
		token.mValue = *mCursor.getCharAtCursor();
		mCursor.tryAdvance();
	}
	else if (mCursor.is(':'))
	{
		token.mType = TokenType::COLON;
		token.mValue = *mCursor.getCharAtCursor();
		mCursor.tryAdvance();
	}
	else if (mCursor.is(','))
	{
		token.mType = TokenType::COMMA;
		token.mValue = *mCursor.getCharAtCursor();
		mCursor.tryAdvance();
	}
	else if (mCursor.isScopeBegin())
	{
		token.mType = TokenType::SCOPE_BEGIN;
		token.mValue = *mCursor.getCharAtCursor();
		mCursor.tryAdvance();
	}
	else if (mCursor.isScopeEnd())
	{
		token.mType = TokenType::SCOPE_END;
		token.mValue = *mCursor.getCharAtCursor();
		mCursor.tryAdvance();
	}
	else
	{
		const auto statementIt = std::find_if(mStatements.begin(), mStatements.end(), [&](const auto& statement) {
			return mCursor.is(statement);
		});

		if (statementIt != mStatements.end())
		{
			const auto& statement = *statementIt;
			token.mType = TokenType::STATEMENT;
			token.mValue = statement;
			mCursor.tryAdvance(statement.size());
		}
		else if (mCursor.isOperator())
		{
			token.mType = TokenType::OPERATOR;
			token.mValue = *mCursor.getCharAtCursor();
			mCursor.tryAdvance();
		}
		else if (mCursor.isAlpha() || mCursor.is('_'))
		{
			token.mType = TokenType::IDENTIFIER;
			do
			{
				token.mValue += *mCursor.getCharAtCursor();
				if (!mCursor.tryAdvance())
				{
					break;
				}
			}
			while (mCursor.isAlpha() || mCursor.isDigit() || mCursor.is('_'));
		}
	}

	return token;
}

}
