#pragma once

#include <erm/modules_lib/Concepts.h>
#include <erm/stl/utils.h>

#include <refl.hpp>

namespace erm {

template<concepts::MainModuleC MainModuleT>
class ModulesManager
{
private:
	template<concepts::ModuleC T>
	static constexpr auto buildDepsTypeList(auto value);

public:
	using ModulesListT = refl::trait::unique_t<decltype(buildDepsTypeList<MainModuleT>(refl::type_list<MainModuleT>{}))>;
	using ModulesTupleT = stl::tuple_from_type_list_t<ModulesListT>;

public:
	ModulesManager() = default;

	int run(int argc, char** argv)
	{
		return std::get<MainModuleT>(mModules).run(argc, argv);
	}
	
private:
	ModulesTupleT mModules;
	
};

template<concepts::MainModuleC MainModuleT>
template<concepts::ModuleC T>
constexpr auto ModulesManager<MainModuleT>::buildDepsTypeList(auto value)
{
	return stl::accumulate_type(
		T::kDependenciesTypeList,
		value,
		[]<concepts::ModuleC M>(auto value) {
			using MT = refl::trait::prepend_t<M, decltype(value)>;
			return buildDepsTypeList<M>(MT{});
		}
	);
}

}
