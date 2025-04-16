#pragma once

@ERM_GENERATED_FILE_WARNING@

#include <refl.hpp>

namespace erm {
@ModuleDependenciesForwardDeclarations@
}

namespace erm {

class @ModuleName@
{
public:
	using DepsT = refl::type_list<@ModuleDependenciesList@>;
	static constexpr auto kDependencies = refl::util::reflect_types(DepsT{});

	static void init();

    static bool visited()
    {
        const auto& instance = getInstance();
        return instance.mVisited;
    }

    static void markVisited()
    {
        auto& instance = getInstance();
        instance.mVisited = true;
    }

    static bool initialized()
    {
        const auto& instance = getInstance();
        return instance.mInitialized;
    }

    static void markInitialized()
    {
        auto& instance = getInstance();
        instance.mInitialized = true;
    }

private:
    @ModuleName@() = default;

    static @ModuleName@& getInstance()
    {
        static @ModuleName@ sInstance;
        return sInstance;
    }

	bool mInitialized = false;
	bool mVisited = false;
};

}

REFL_AUTO(
    type(erm::@ModuleName@)
);
