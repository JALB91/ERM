#pragma once

#include <erm/math/Types.h>

#include <erm/utils/StringID.h>

#include <refl.hpp>

namespace erm {

using TypeID = StringID::HashT;

template<typename T>
inline constexpr TypeID getID()
{
	static_assert(refl::is_reflectable<T>());
	constexpr auto t = refl::reflect<T>();
	static auto hash = StringID(t.name.c_str()).getHash();
	return hash;
}

}
