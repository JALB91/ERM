#include "erm/nn/Tokenizer.h"

#include <erm/log/Assert.h>

#include <erm/utils/Utils.h>

#include <algorithm>

namespace erm::nn {

Tokenizer::Tokenizer(std::string_view text) noexcept
	: mCursor(text)
{}

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
		token.mValue += *mCursor.getCharAtCursor();
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
		
		token.mValue += *mCursor.getCharAtCursor();
		token.mType = TokenType::STRING_LITERAL;
		mCursor.tryAdvance();
	}
	else if (mCursor.isSymbol())
	{
		token.mType = TokenType::SYMBOL;
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

	return token;
}

}
