#pragma once

#include "erm/utils/Utils.h"

#include <type_traits>
#include <cstddef>

namespace erm::utils {

namespace internal {

size_t GetNextID();

template<typename T>
size_t GetIDImpl()
{
	static const size_t sID = GetNextID();
	return sID;
}

}

template<typename T>
size_t GetID()
{
	return internal::GetIDImpl<utils::RemoveAll_t<T>>();
}

}
