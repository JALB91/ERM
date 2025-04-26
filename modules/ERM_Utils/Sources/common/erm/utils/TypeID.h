#pragma once

#include "erm/utils/Utils.h"

#include <erm/math/Types.h>

#include <type_traits>
#include <cstddef>

namespace erm::utils {

using TypeID = u64;

namespace internal {

TypeID getNextID();

template<typename T>
inline TypeID getIDImpl()
{
	static const auto sID = getNextID();
	return sID;
}

}

template<typename T>
inline constexpr TypeID getID()
{
	return internal::getIDImpl<utils::remove_all_t<T>>();
}

}
