#pragma once

#include <erm/AllModules.h>

#include <erm/modules/Concepts.h>

#include <erm/stl/utils.h>

#include <erm/type_id/TypeID.h>

#include <refl.hpp>

#include <unordered_map>

namespace erm {

template<concepts::MainModule M>
class ModulesManager
{
public:
	using MainModuleT = M;

public:
	ModulesManager() = default;

	bool init()
	{
		return initModule<MainModuleT>();
	}

	bool deinit()
	{
		return destroyModule<MainModuleT>();
	}

	int run(int argc, char** argv)
	{
		return MainModuleT::run(argc, argv);
	}
	
private:
	template<concepts::Module T, typename F>
	auto visitModuleRecursive(const F& function)
	{
		const auto typeID = getID<T>();
		const bool visited = stl::find_or(mVisitedModules, typeID, false);
		
		if (visited)
		{
			return function.template operator()<T>();
		}

		mVisitedModules[typeID] = true;

		stl::for_each_type(T::kDependencies, [this, &function]<concepts::Module Dependency>() {
			visitModuleRecursive<Dependency>(function);
		});

		return function.template operator()<T>();
	}

	template<concepts::Module T>
	bool initModule()
	{
		mVisitedModules.clear();

		return visitModuleRecursive<T>([]<concepts::Module T>() {
			return T::initialized() || T::init();
		});
	}

	template<concepts::Module T>
	bool destroyModule()
	{
		mVisitedModules.clear();

		return visitModuleRecursive<T>([]<concepts::Module T>() {
			return !T::initialized() || T::deinit();
		});
	}

	std::unordered_map<TypeID, bool> mVisitedModules;
	
};

}
