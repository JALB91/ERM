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

    Iterator(pointer ptr)
		: mPtr(ptr)
	{}

    reference operator*() const { return *mPtr; }
	pointer operator->() { return mPtr; }

	Iterator& operator++()
	{
		mPtr++;
		return *this;
	}

	Iterator operator++(int)
	{
		Iterator tmp = *this;
		++(*this);
		return tmp;
	}

	friend bool operator==(const Iterator& a, const Iterator& b) { return a.mPtr == b.mPtr; };
	friend bool operator!=(const Iterator& a, const Iterator& b) { return a.mPtr != b.mPtr; }; 

private:
	pointer mPtr;

};

}