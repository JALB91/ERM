#pragma once

#include "erm/utils/Iterator.h"

#include <erm/math/Types.h>

#include <cstdarg>
#include <string_view>

namespace erm {

template<u16 SIZE>
class StaticString
{
public:
    StaticString() noexcept
		: mStr("\0")
	{}

	StaticString(const char* str)
	{
		strncpy(mStr, str, std::max(static_cast<u64>(SIZE - 1), strlen(str)));
		mStr[std::min(static_cast<u64>(SIZE - 1), strlen(str))] = '\0';
	}

    StaticString(std::string_view fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		vsnprintf(mStr, SIZE, fmt.data(), args);
		va_end(args);
	}

	StaticString& operator=(const char* str)
	{
		strncpy(mStr, str, std::max(static_cast<u64>(SIZE - 1), strlen(str)));
		mStr[std::min(static_cast<u64>(SIZE - 1), strlen(str))] = '\0';
		return *this;
	}
	
    StaticString& operator=(std::string_view str)
	{
		strncpy(mStr, str.data(), std::max(static_cast<u64>(SIZE - 1), str.size()));
		mStr[std::min(static_cast<u64>(SIZE - 1), str.size())] = '\0';
		return *this;
	}

	StaticString& operator=(char c)
	{
		static_assert(SIZE > 1);
		mStr[0] = c;
		mStr[1] = '\0';
		return *this;
	}

	StaticString& operator+=(std::string_view str)
	{
		append(str);
		return *this;
	}

	StaticString& operator+=(char c)
	{
		const u16 currSize = size();
		if (currSize < SIZE - 1)
		{
			mStr[currSize] = c;
			mStr[currSize + 1] = '\0';
		}

		return *this;
	}

    operator std::string_view() const
	{
		return std::string_view(data(), size());
	}

	operator std::string_view()
	{
		return std::string_view(data(), size());
	}

    bool operator==(std::string_view value) const
	{
		if (size() != value.size())
		{
			return false;
		}

		return strncmp(mStr, value.data(), size()) == 0;
	}

    void append(std::string_view fmt, ...)
	{
		if (size() == SIZE)
		{
			return;
		}

		va_list args;
		va_start(args, fmt);
		vsnprintf(mStr[size()], SIZE - size(), fmt.data(), args);
		va_end(args);
	}

	void clear()
	{
		mStr[0] = '\0';
	}

	std::string_view substr(u16 from, u16 count) const
	{
		return std::string_view(mStr[from], count);
	}

    bool empty() const { return mStr[0] == '\0'; }
	u16 capacity() const { return SIZE; }
	u16 size() const { return static_cast<u16>(strlen(data())); }
    const char* data() const { return mStr; }

	Iterator<char> begin() const { return Iterator(&mStr[0]); }
	Iterator<char> end() const { return Iterator(&mStr[SIZE]); }

private:
    char mStr[SIZE];

};

using str16 = StaticString<16>;
using str32 = StaticString<32>;
using str64 = StaticString<64>;
using str128 = StaticString<128>;
using str256 = StaticString<256>;

}