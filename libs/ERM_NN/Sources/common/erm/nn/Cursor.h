#pragma once

#include <erm/math/Types.h>

#include <optional>
#include <string>
#include <string_view>

namespace erm::nn {
	
class Cursor
{
public:
	Cursor(std::string_view text);
	
	void init(std::string_view text);
	
	bool tryAdvance(u64 offset = 1);
	
	std::optional<char> getCharAtCursor() const;
	std::optional<char> getCharAfterCursor() const;
	bool isMultilineCommentBegin() const;
	bool isMultilineCommentEnd() const;
	bool isOneLineComment() const;
	bool isSpaceOrTab() const;
	bool isNewLine() const;
	bool isSymbol() const;
	bool isDigit() const;
	bool isAlpha() const;
	bool isValid() const;
	bool is(char c) const;
	bool is(std::string_view str) const;
	
	inline u16 getCursorLine() const
	{
		return mCursorLine;
	}
	
	inline u16 getCursorLineOffset() const
	{
		return mCursorLineOffset;
	}
	
private:
	std::string mText;
	u64 mCursorOffset;
	u16 mCursorLine;
	u16 mCursorLineOffset;
		
};
	
}

