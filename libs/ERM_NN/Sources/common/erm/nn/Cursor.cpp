#include "erm/nn/Cursor.h"

namespace erm::nn {

Cursor::Cursor()
	: mCursorOffset(0)
	, mCursorLine(0)
	, mCursorLineOffset(0)
{}

void Cursor::init(std::string_view text)
{
	mText = text;
	mCursorOffset = 0;
	mCursorLine = 0;
	mCursorLineOffset = 0;
}

bool Cursor::tryAdvance(u64 offset /* = 1*/)
{
	if (!isValid())
	{
		return false;
	}
	
	for (u64 i = 0; i < offset; ++i)
	{
		const bool newLine = isNewLine();
		++mCursorOffset;
		mCursorLine += (newLine ? 1 : 0);
		mCursorLineOffset = (newLine ? 0 : mCursorLineOffset + 1);
		
		if (!isValid())
		{
			return false;
		}
	}
	
	return true;
}

std::optional<char> Cursor::getCharAtCursor() const
{
	return isValid() ? std::make_optional(mText[mCursorOffset]) : std::nullopt;
}

std::optional<char> Cursor::getCharAfterCursor() const
{
	return (mCursorOffset + 1 < mText.size()) ? std::make_optional(mText[mCursorOffset + 1]) : std::nullopt;
}

bool Cursor::isMultilineCommentBegin() const
{
	return getCharAtCursor() == '/' && getCharAfterCursor() == '*';
}

bool Cursor::isMultilineCommentEnd() const
{
	return getCharAtCursor() == '*' && getCharAfterCursor() == '/';
}

bool Cursor::isOneLineComment() const
{
	return getCharAtCursor() == '/' && getCharAfterCursor() == '/';
}

bool Cursor::isScopeBegin() const
{
	const auto currChar = getCharAtCursor();
	return 
		currChar == '{' ||
		currChar == '[' ||
		currChar == '(' ||
		currChar == '<';
}

bool Cursor::isScopeEnd() const
{
	const auto currChar = getCharAtCursor();
	return 
		currChar == '}' ||
		currChar == ']' ||
		currChar == ')' ||
		currChar == '>';
}

bool Cursor::isSpaceOrTab() const
{
	const auto currChar = getCharAtCursor();
	return currChar == ' ' || currChar == '\t';
}

bool Cursor::isOperator() const
{
	const auto currChar = getCharAtCursor();
	return 
		currChar == '=' ||
		currChar == '+' ||
		currChar == '-' ||
		currChar == '*' ||
		currChar == '/' ||
		currChar == '&' ||
		currChar == '|' ||
		currChar == '^' ||
		currChar == '~' ||
		currChar == '!';
}

bool Cursor::isNewLine() const
{
	return getCharAtCursor() == '\n';
}

bool Cursor::isDigit() const
{
	return isValid() && isdigit(mText[mCursorOffset]) != 0;
}

bool Cursor::isAlpha() const
{
	return isValid() && isalpha(mText[mCursorOffset]) != 0;
}

bool Cursor::isValid() const
{
	return mCursorOffset < mText.size();
}

bool Cursor::is(char c) const
{
	return getCharAtCursor() == c;
}

bool Cursor::is(std::string_view str) const
{
	return isValid() && mCursorOffset + str.size() < mText.size() && mText.substr(mCursorOffset, str.size()) == str;
}

}
