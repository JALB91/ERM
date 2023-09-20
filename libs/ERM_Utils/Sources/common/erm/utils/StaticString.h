#pragma once

#include "erm/utils/Iterator.h"
#include "erm/utils/Utils.h"

#include <erm/math/Types.h>

#include <cstdarg>
#include <string_view>

namespace erm {

template<u16 SIZE>
class StaticString
{
public:
    constexpr StaticString() noexcept
	{
		mStr[0] = '\0';
	}

	constexpr StaticString(const char* str) noexcept
	{
		const u64 strSize = utils::c_str_size(str);
		const u64 copySize = std::min(static_cast<u64>(SIZE - 1), strSize);
		u64 index = 0;
		while (index < copySize)
		{
			mStr[index] = str[index];
			++index;
		}
		mStr[index] = '\0';
	}

	StaticString(std::string_view fmt, ...) noexcept
	{
		va_list args;
		va_start(args, fmt);
		vsnprintf(mStr, SIZE, fmt.data(), args);
		va_end(args);
	}

	constexpr StaticString& operator=(const char* str) noexcept
	{
		const u64 copySize = std::min(static_cast<u64>(SIZE - 1), utils::c_str_size(str));
		u64 index = 0;
		while (index < copySize)
		{
			mStr[index] = str[index];
			++index;
		}
		mStr[index] = '\0';
		return *this;
	}

	constexpr StaticString& operator=(std::string_view str) noexcept
	{
		const u64 copySize = std::min(static_cast<u64>(SIZE - 1), str.size());
		u64 index = 0;
		while (index < copySize)
		{
			mStr[index] = str[index];
			++index;
		}
		mStr[index] = '\0';
		return *this;
	}

	constexpr StaticString& operator=(char c) noexcept
	{
		static_assert(SIZE > 1);
		mStr[0] = c;
		mStr[1] = '\0';
		return *this;
	}

	constexpr StaticString& operator+=(std::string_view str) noexcept
	{
		const u16 currSize = size();
		const u64 strSize = str.size();
		const u64 finalSize = currSize + strSize;
		u64 index = currSize;
		while (index < SIZE && index < finalSize)
		{
			mStr[index] = str[index - currSize];
			++index;
		}
		mStr[index] = '\0';
		return *this;
	}

	constexpr StaticString& operator+=(char c) noexcept
	{
		const u16 currSize = size();
		if (currSize < SIZE - 1)
		{
			mStr[currSize] = c;
			mStr[currSize + 1] = '\0';
		}
		return *this;
	}

	constexpr operator std::string_view() const noexcept
	{
		return std::string_view(data(), size());
	}

	constexpr operator std::string_view() noexcept
	{
		return std::string_view(data(), size());
	}

	constexpr bool operator==(std::string_view str) const noexcept
	{
		if (size() != str.size())
		{
			return false;
		}

		for (u16 index = 0; index < size(); ++index)
		{
			if (mStr[index] != str[index])
			{
				return false;
			}
		}

		return true;
	}

	template<u16 S>
	constexpr bool operator==(const StaticString<S>& str) const noexcept
	{
		if (size() != str.size())
		{
			return false;
		}

		for (u16 index = 0; index < size(); ++index)
		{
			if (mStr[index] != str[index])
			{
				return false;
			}
		}

		return true;
	}

	constexpr char operator[](u16 index) const noexcept
	{
		return mStr[index];
	}

	constexpr char& operator[](u16 index) noexcept
	{
		return mStr[index];
	}

	void append(std::string_view fmt, ...) noexcept
	{
		const u16 currSize = size();
		if (currSize >= SIZE)
		{
			return;
		}

		va_list args;
		va_start(args, fmt);
		vsnprintf(mStr[currSize], SIZE - currSize, fmt.data(), args);
		va_end(args);
	}

	constexpr void clear() noexcept
	{
		mStr[0] = '\0';
	}

	constexpr void toLower() noexcept
	{
		for (u16 index = 0; index < size(); ++index)
		{
			mStr[index] = utils::charToLower(mStr[index]);
		}
	}

	constexpr void toUpper() noexcept
	{
		for (u16 index = 0; index < size(); ++index)
		{
			mStr[index] = utils::charToUpper(mStr[index]);
		}
	}

	constexpr std::string_view substr(u16 from, u16 count) const noexcept
	{
		return std::string_view(&mStr[std::min(SIZE, from)], std::min(static_cast<u16>(SIZE - from), count));
	}

	constexpr u16 size() const noexcept
	{
		return static_cast<u16>(utils::c_str_size(mStr));
	}

    constexpr bool empty() const noexcept { return mStr[0] == '\0'; }
	constexpr u16 capacity() const noexcept { return SIZE; }
	constexpr const char* data() const noexcept { return mStr; }
	constexpr char* data() noexcept { return mStr; }

	constexpr Iterator<const char> cbegin() const noexcept { return Iterator(&mStr[0]); }
	constexpr Iterator<const char> cend() const noexcept { return Iterator(&mStr[size()]); }

	constexpr Iterator<char> begin() noexcept { return Iterator(&mStr[0]); }
	constexpr Iterator<char> end() noexcept { return Iterator(&mStr[size()]); }

private:
    char mStr[SIZE];

};

using str16 = StaticString<16>;
using str32 = StaticString<32>;
using str64 = StaticString<64>;
using str128 = StaticString<128>;
using str256 = StaticString<256>;

}