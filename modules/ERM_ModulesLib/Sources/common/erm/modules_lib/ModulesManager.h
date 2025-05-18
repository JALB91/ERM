#pragma once

#include <erm/modules_lib/Concepts.h>
#include <erm/stl/utils.h>

#include <refl.hpp>

namespace erm {

template<concepts::MainModule M>
class ModulesManager
{
private:
	template<concepts::Module T>
	static constexpr auto buildDepsTypeList(auto value);

public:
	using MainModuleT = M;
	
	using DepsListT = decltype(buildDepsTypeList<MainModuleT>(refl::type_list<MainModuleT>{}));
	static constexpr auto kDepsList = DepsListT {};

	using RevDepsListT = refl::trait::reverse_t<DepsListT>;
	static constexpr auto kRevDepsList = RevDepsListT {};

public:
	ModulesManager() = default;

	bool init()
	{
		return stl::accumulate_type(
			kDepsList,
			true,
			[]<concepts::Module Module>(bool value) {
				return value && Module::init();
			});
	}

	bool deinit()
	{
		return stl::accumulate_type(
			kRevDepsList,
			true,
			[]<concepts::Module Module>(bool value) {
				return value && Module::deinit();
			});
	}

	int run(int argc, char** argv)
	{
		return MainModuleT::run(argc, argv);
	}
	
};

template<concepts::MainModule MainModule>
template<concepts::Module T>
constexpr auto ModulesManager<MainModule>::buildDepsTypeList(auto value)
{
	static_assert(refl::util::contains<T>(value));

	if constexpr (T::DepsT::size > 0)
	{
		return stl::accumulate_type(
			T::kDependencies,
			value,
			[]<concepts::Module Dependency>(auto value) {
				if constexpr (!refl::util::contains<Dependency>(value))
				{
					return buildDepsTypeList<Dependency>(refl::trait::prepend_t<Dependency, decltype(value)> {});
				}
				else
				{
					constexpr auto filtered = stl::filter_type(value, []<concepts::Module S>() {
						return !std::is_same_v<Dependency, S>;
					});
					constexpr auto replaced = refl::trait::prepend_t<Dependency, std::remove_const_t<decltype(filtered)>> {};
					return replaced;
				}
			});
	}
	else
	{
		return value;
	}
}

}
