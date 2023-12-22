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
	
	static_assert(SIZE > 1);
	static_assert(sizeof(size_type) <= sizeof(size_t));
	
	static constexpr size_type npos = std::numeric_limits<size_type>::max();

public:
	constexpr StaticString(char ch = '\0', u16 count = 1) noexcept
		: mStr{'\0'}
	{
		for (u16 i = 0; i < count; ++i)
		{
			mStr[i] = ch;
		}
	}

	constexpr StaticString(const char* const str) noexcept
		: mStr{'\0'}
	{
		if (str == nullptr)
		{
			return;
		}

		const size_t strSize = utils::c_str_size(str);
		const size_t copySize = std::min(static_cast<size_t>(SIZE - 1), strSize);
		size_t index = 0;
		while (index < copySize)
		{
			mStr[index] = str[index];
			++index;
		}
	}

	constexpr StaticString(std::string_view str) noexcept
		: mStr{'\0'}
	{
		const size_t strSize = str.size();
		const size_t copySize = std::min(static_cast<size_t>(SIZE - 1), strSize);
		size_t index = 0;
		while (index < copySize)
		{
			mStr[index] = str[index];
			++index;
		}
	}

	constexpr StaticString& operator=(char c) noexcept
	{
		clear();
		mStr[0] = c;
		return *this;
	}

	constexpr StaticString& operator=(const char* const str) noexcept
	{
		clear();
		
		if (str == nullptr)
		{
			return *this;
		}

		const size_t copySize = std::min(static_cast<size_t>(SIZE - 1), utils::c_str_size(str));
		size_t index = 0;
		while (index < copySize)
		{
			mStr[index] = str[index];
			++index;
		}
		return *this;
	}

	constexpr StaticString& operator=(std::string_view str) noexcept
	{
		clear();
		
		const size_t copySize = std::min(static_cast<size_t>(SIZE - 1), str.size());
		size_t index = 0;
		while (index < copySize)
		{
			mStr[index] = str[index];
			++index;
		}
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
		const size_t strSize = utils::c_str_size(str);
		const size_t finalSize = currSize + strSize;
		size_t index = currSize;
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
		const size_t strSize = str.size();
		const size_t finalSize = currSize + strSize;
		size_t index = currSize;
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

	constexpr const char& front() const noexcept
	{
		return mStr[0];
	}

	constexpr const char& back() const noexcept
	{
		return mStr[size() - 1];
	}

	constexpr char& front() noexcept
	{
		return mStr[0];
	}

	constexpr char& back() noexcept
	{
		return mStr[size() - 1];
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
		const size_t otherSize = str.size();

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
		const size_t otherSize = str.size();

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

	constexpr bool startsWith(char ch) const noexcept
	{
		if (empty())
		{
			return false;
		}

		return mStr[0] == ch;
	}

	constexpr bool startsWith(std::string_view str) const noexcept
	{
		const size_t strSize = str.size();
		if (size() < strSize)
		{
			return false;
		}

		for (size_t i = 0; i < strSize; ++i)
		{
			if (mStr[i] != str[i])
			{
				return false;
			}
		}

		return true;
	}

	constexpr bool endsWith(char ch) const noexcept
	{
		if (empty())
		{
			return false;
		}

		return mStr[size() - 1] == ch;
	}

	constexpr bool endsWith(std::string_view str) const noexcept
	{
		const size_t mySize = size();
		const size_t strSize = str.size();

		if (mySize < strSize || str.empty())
		{
			return false;
		}

		for (size_t i = mySize - strSize; i < mySize; ++i)
		{
			if (mStr[i] != str[i - (mySize - strSize)])
			{
				return false;
			}
		}

		return true;
	}

	constexpr StaticString& clear() noexcept
	{
		memset(mStr, '\0', SIZE - 1);
		return *this;
	}
	
	constexpr StaticString& toLower() noexcept
	{
		for (size_type index = 0; index < size(); ++index)
		{
			mStr[index] = utils::charToLower(mStr[index]);
		}
		return *this;
	}

	constexpr StaticString toLower() const noexcept
	{
		StaticString result;
		for (size_type index = 0; index < size(); ++index)
		{
			result.mStr[index] = utils::charToLower(mStr[index]);
		}
		return result;
	}
	
	constexpr StaticString& toUpper() noexcept
	{
		for (size_type index = 0; index < size(); ++index)
		{
			mStr[index] = utils::charToUpper(mStr[index]);
		}
		return *this;
	}

	constexpr StaticString toUpper() const noexcept
	{
		StaticString result;
		for (size_type index = 0; index < size(); ++index)
		{
			result.mStr[index] = utils::charToUpper(mStr[index]);
		}
		return result;
	}
	
	constexpr StaticString& trim() noexcept
	{
		if (empty())
		{
			return *this;
		}

		size_type currSize = size();
		while (currSize > 0 && mStr[--currSize] == ' ')
		{
			mStr[currSize] = '\0';
		}

		size_type nStartSpaces = 0;
		while (mStr[nStartSpaces] == ' ')
		{
			++nStartSpaces;
		}

		if (nStartSpaces > 0)
		{
			for (size_type i = nStartSpaces; i <= currSize; ++i)
			{
				mStr[i - nStartSpaces] = mStr[i];
			}
			mStr[(currSize + 1) - nStartSpaces] = '\0';
		}

		return *this;
	}

	constexpr StaticString trim() const noexcept
	{
		StaticString result = data();
		if (result.empty())
		{
			return result;
		}

		size_type currSize = result.size();
		while (currSize > 0 && result.mStr[--currSize] == ' ')
		{
			result.mStr[currSize] = '\0';
		}

		size_type nStartSpaces = 0;
		while (result.mStr[nStartSpaces] == ' ')
		{
			++nStartSpaces;
		}

		if (nStartSpaces > 0)
		{
			for (size_type i = nStartSpaces; i <= currSize; ++i)
			{
				result.mStr[i - nStartSpaces] = result.mStr[i];
			}
			result.mStr[(currSize + 1) - nStartSpaces] = '\0';
		}

		return result;
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

	friend std::istream& operator>>(std::istream& is, StaticString& str)
	{
		is >> str.mStr;
		return is;
	}

	void format(const char* const fmt, ...) noexcept
	{
		va_list args;
		va_start(args, fmt);
		vsnprintf(&mStr[0], SIZE, fmt, args);
		va_end(args);
	}
	
	constexpr void append(char c) noexcept
	{
		const size_type currSize = size();
		if (currSize >= SIZE)
		{
			return;
		}
		
		mStr[currSize] = c;
		mStr[currSize + 1] = '\0';
	}

	void append(const char* const fmt, ...) noexcept
	{
		const size_type currSize = size();
		if (currSize >= SIZE)
		{
			return;
		}

		va_list args;
		va_start(args, fmt);
		vsnprintf(&mStr[currSize], SIZE - currSize, fmt, args);
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
using str512 = StaticString<512>;
using str1024 = StaticString<1024>;

}
