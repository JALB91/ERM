#pragma once

#include <expected>
#include <string>

namespace erm::concepts {

template<typename T>
concept ModuleC = requires(T type)
{
	T::kDependenciesTypeList;
};

template<typename T>
concept MainModuleC = requires(T type, int argc, char** argv)
{
	T::kDependenciesTypeList;
	{ type.run(argc, argv) } -> std::same_as<int>;
};

}
