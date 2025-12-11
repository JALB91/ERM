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
	using ModulesPackT = stl::pack_from_type_list_t<ModulesListT>;

public:
	ModulesManager() noexcept = default;

	int run(int argc, char** argv)
	{
		return mModules.require<MainModuleT>().run(argc, argv);
	}
	
private:
	ModulesPackT mModules;
	
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
