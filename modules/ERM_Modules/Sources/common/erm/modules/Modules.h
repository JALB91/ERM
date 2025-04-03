#pragma once

#include <refl.hpp>

namespace erm {

template<typename... Ts>
struct TypeList {};

template<typename... Ts>
struct ModuleType : public refl::attr::usage::type
{
	using DepsT = TypeList<Ts...>;
	
	constexpr ModuleType(DepsT dependencies)
		: mDependencies(dependencies)
	{}
	
	DepsT mDependencies;
	
};

}
