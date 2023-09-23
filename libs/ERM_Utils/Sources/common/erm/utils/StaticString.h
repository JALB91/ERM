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
	using size_type = decltype(SIZE);
	static constexpr size_type npos = std::numeric_limits<size_type>::max();

	static StaticString format(const char* fmt, ...) noexcept
	{
		StaticString result;
		va_list args;
		va_start(args, fmt);
		vsnprintf(result.mStr, SIZE, fmt, args);
		va_end(args);
		return result;
	}

public:
	constexpr StaticString(char ch = '\0') noexcept
	{
		mStr[0] = ch;
	}

	constexpr StaticString(const char* const str) noexcept
	{
		if (str == nullptr)
		{
			clear();
			return;
		}

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

	constexpr StaticString(std::string_view str) noexcept
	{
		const u64 strSize = str.size();
		const u64 copySize = std::min(static_cast<u64>(SIZE - 1), strSize);
		u64 index = 0;
		while (index < copySize)
		{
			mStr[index] = str[index];
			++index;
		}
		mStr[index] = '\0';
	}

	constexpr StaticString& operator=(char c) noexcept
	{
		static_assert(SIZE > 1);
		mStr[0] = c;
		mStr[1] = '\0';
		return *this;
	}

	constexpr StaticString& operator=(const char* const str) noexcept
	{
		if (str == nullptr)
		{
			clear();
			return *this;
		}

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

	constexpr StaticString& operator+=(char c) noexcept
	{
		const size_type currSize = size();
		if (currSize < SIZE - 1)
		{
			mStr[currSize] = c;
			mStr[currSize + 1] = '\0';
		}
		return *this;
	}

	constexpr StaticString& operator+=(const char* const str) noexcept
	{
		if (str == nullptr)
		{
			return *this;
		}

		const size_type currSize = size();
		const u64 strSize = utils::c_str_size(str);
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

	constexpr StaticString& operator+=(std::string_view str) noexcept
	{
		const size_type currSize = size();
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

	constexpr operator std::string_view() const noexcept
	{
		return std::string_view(data(), size());
	}

	constexpr operator std::string_view() noexcept
	{
		return std::string_view(data(), size());
	}

	constexpr bool operator==(char ch) const noexcept
	{
		return mStr[0] == ch && mStr[1] == '\0';
	}

	constexpr bool operator==(const char* const str) const noexcept
	{
		if (str == nullptr)
		{
			return false;
		}

		const size_type currSize = size();
		if (currSize != utils::c_str_size(str))
		{
			return false;
		}

		for (size_type index = 0; index < currSize; ++index)
		{
			if (mStr[index] != str[index])
			{
				return false;
			}
		}

		return true;
	}

	constexpr bool operator==(std::string_view str) const noexcept
	{
		const size_type currSize = size();
		if (currSize != str.size())
		{
			return false;
		}

		for (size_type index = 0; index < currSize; ++index)
		{
			if (mStr[index] != str[index])
			{
				return false;
			}
		}

		return true;
	}

	template<size_type S>
	constexpr bool operator==(const StaticString<S>& str) const noexcept
	{
		const size_type currSize = size();
		if (currSize != str.size())
		{
			return false;
		}

		for (size_type index = 0; index < currSize; ++index)
		{
			if (mStr[index] != str[index])
			{
				return false;
			}
		}

		return true;
	}

	constexpr char operator[](size_type index) const noexcept
	{
		return mStr[index];
	}

	constexpr char& operator[](size_type index) noexcept
	{
		return mStr[index];
	}

	constexpr size_type find(char ch) const noexcept
	{
		for (size_type index = 0; index < size(); ++index)
		{
			if (mStr[index] == ch)
			{
				return index;
			}
		}

		return npos;
	}

	constexpr size_type rfind(char ch) const noexcept
	{
		if (empty())
		{
			return npos;
		}

		size_type index = size();
		do
		{
			if (mStr[--index] == ch)
			{
				return index;
			}
		} while (index > 0);

		return npos;
	}

	constexpr size_type find(std::string_view str) const noexcept
	{
		const size_type currSize = size();
		const u64 otherSize = str.size();

		if (currSize < otherSize)
		{
			return npos;
		}

		for (size_type index = 0; index < currSize; ++index)
		{
			if (mStr[index] == str[0])
			{
				size_type tmpIndex = index;

				while (tmpIndex - index < otherSize && mStr[tmpIndex] == str[tmpIndex - index])
				{
					++tmpIndex;
				}

				if (tmpIndex - index == otherSize)
				{
					return index;
				}
			}
		}

		return npos;
	}

	constexpr size_type rfind(std::string_view str) const noexcept
	{
		const size_type currSize = size();
		const u64 otherSize = str.size();

		if (currSize < otherSize)
		{
			return npos;
		}

		size_type index = currSize - 1;

		while (index >= 0)
		{
			if (mStr[index] == str[0])
			{
				size_type tmpIndex = index;

				while (tmpIndex - index < otherSize && mStr[tmpIndex] == str[tmpIndex - index])
				{
					++tmpIndex;
				}

				if (tmpIndex - index == otherSize)
				{
					return index;
				}
			}

			if (index == 0)
			{
				break;
			}

			--index;
		}

		return npos;
	}

	constexpr bool startsWith(std::string_view str) const noexcept
	{
		if (size() < str.size())
		{
			return false;
		}

		for (size_type i = 0; i < static_cast<size_type>(str.size()); ++i)
		{
			if (mStr[i] != str[i])
			{
				return false;
			}
		}

		return true;
	}

	constexpr bool endsWith(std::string_view str) const noexcept
	{
		const u64 mySize = size();
		const u64 strSize = str.size();

		if (mySize < strSize || str.empty())
		{
			return false;
		}

		for (u64 i = mySize - strSize; i < mySize; ++i)
		{
			if (mStr[i] != str[i - (mySize - strSize)])
			{
				return false;
			}
		}

		return true;
	}

	constexpr void clear() noexcept
	{
		mStr[0] = '\0';
	}

	constexpr void toLower() noexcept
	{
		for (size_type index = 0; index < size(); ++index)
		{
			mStr[index] = utils::charToLower(mStr[index]);
		}
	}

	constexpr void toUpper() noexcept
	{
		for (size_type index = 0; index < size(); ++index)
		{
			mStr[index] = utils::charToUpper(mStr[index]);
		}
	}

	constexpr std::string_view substr(size_type from, size_type count) const noexcept
	{
		return std::string_view(&mStr[from], count);
	}

	constexpr size_type size() const noexcept
	{
		return static_cast<size_type>(utils::c_str_size(mStr));
	}

	constexpr bool empty() const noexcept { return mStr[0] == '\0'; }
	constexpr size_type capacity() const noexcept { return SIZE; }
	constexpr const char* data() const noexcept { return mStr; }
	constexpr char* data() noexcept { return mStr; }

	constexpr Iterator<const char> cbegin() const noexcept { return Iterator(&mStr[0]); }
	constexpr Iterator<const char> cend() const noexcept { return Iterator(&mStr[size()]); }

	constexpr Iterator<char> begin() noexcept { return Iterator(&mStr[0]); }
	constexpr Iterator<char> end() noexcept { return Iterator(&mStr[size()]); }

	friend std::ostream& operator<<(std::ostream& os, const StaticString& str)
	{
		os << str.mStr;
		return os;
	}

	void append(std::string_view fmt, ...) noexcept
	{
		const size_type currSize = size();
		if (currSize >= SIZE)
		{
			return;
		}

		va_list args;
		va_start(args, fmt);
		vsnprintf(mStr[currSize], SIZE - currSize, fmt.data(), args);
		va_end(args);
	}

private:
    char mStr[SIZE];

};

using str16 = StaticString<16>;
using str32 = StaticString<32>;
using str64 = StaticString<64>;
using str128 = StaticString<128>;
using str256 = StaticString<256>;

}