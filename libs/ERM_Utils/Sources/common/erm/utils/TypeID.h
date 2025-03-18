#pragma once

#include "erm/utils/Utils.h"

#include <erm/math/Types.h>

#include <type_traits>
#include <cstddef>

namespace erm::utils {

namespace internal {

u64 getNextID();

template<typename T>
inline u64 getIDImpl()
{
	static const auto sID = getNextID();
	return sID;
}

}

template<typename T>
inline constexpr u64 getID()
{
	return internal::getIDImpl<utils::remove_all_t<T>>();
}

}
