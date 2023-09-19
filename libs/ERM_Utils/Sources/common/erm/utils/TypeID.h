#pragma once

#include "erm/utils/Utils.h"

#include <type_traits>
#include <cstddef>

namespace erm::utils {

namespace internal {

u64 getNextID();

template<typename T>
u64 getIDImpl()
{
	static const u64 sID = getNextID();
	return sID;
}

}

template<typename T>
u64 getID()
{
	return internal::getIDImpl<utils::remove_all_t<T>>();
}

}
