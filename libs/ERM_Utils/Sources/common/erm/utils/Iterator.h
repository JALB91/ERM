#pragma once

#include <cstddef>
#include <iterator>

namespace erm {

template<typename T>
struct Iterator
{
	using iterator_category = std::forward_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = T;
	using pointer           = value_type*;
	using reference         = value_type&;

	constexpr Iterator(pointer ptr) noexcept
		: mPtr(ptr)
	{}

	constexpr reference operator*() const { return *mPtr; }
	constexpr pointer operator->() { return mPtr; }

	constexpr Iterator& operator++()
	{
		++mPtr;
		return *this;
	}

	constexpr Iterator operator++(int)
	{
		Iterator tmp = *this;
		++(*this);
		return tmp;
	}

	friend constexpr bool operator==(const Iterator& a, const Iterator& b) { return a.mPtr == b.mPtr; };
	friend constexpr bool operator!=(const Iterator& a, const Iterator& b) { return a.mPtr != b.mPtr; }; 

private:
	pointer mPtr;

};

}