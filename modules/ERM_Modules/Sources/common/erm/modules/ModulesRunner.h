#pragma once

#include <erm/AllModules.h>

#include <erm/modules/Concepts.h>

#include <refl.hpp>

namespace erm {

class ModulesRunner
{
public:
	template<concepts::Module T>
	static ModulesRunner create()
	{
		refl::util::for_each(T::kDependencies, [](const auto base) {
			using Base = typename decltype(base)::type;
			initModules<Base>();
		});
		
		return ModulesRunner();
	}
	
private:
	ModulesRunner() = default;

	template<concepts::Module T>
	static void initModules()
	{
		if (T::visited())
		{
			if (!T::initialized())
			{
				T::init();
				T::markInitialized();
			}
			return;
		}
		
		T::markVisited();
		
		refl::util::for_each(T::kDependencies, [](const auto base) {
			using Base = typename decltype(base)::type;
			initModules<Base>();
		});
		
		if (!T::initialized())
		{
			T::init();
			T::markInitialized();
		}
	}
	
};

extern ModulesRunner eModulesRunner;

}
