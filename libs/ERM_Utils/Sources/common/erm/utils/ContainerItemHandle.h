#pragma once

#include "erm/utils/assert/Assert.h"

#include <optional>

namespace erm {

template<typename Container, typename Key>
class ContainerItemHandle
{
public:
	ContainerItemHandle()
		: mContainer(nullptr)
	{}
	
	ContainerItemHandle(Container& container, Key key)
		: mContainer(&container)
		, mKey(key)
	{}
	
	Container::value_type* operator->()
	{
		ERM_ASSERT(isValid());
		return &mContainer->at(mKey.value());
	}
	
	const Container::value_type* operator->() const
	{
		return &mContainer->at(mKey.value());
	}
	
	void reset()
	{
		mKey.reset();
		mContainer = nullptr;
	}
	
	bool isValid() const
	{
		return mKey.has_value() && mContainer != nullptr;
	}
	
private:
	Container* mContainer;
	std::optional<Key> mKey;
	
};

template<typename T>
using VectorItemHandle = ContainerItemHandle<std::vector<T>, size_t>;

}

