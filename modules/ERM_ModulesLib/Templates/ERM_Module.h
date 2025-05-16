#pragma once

@ERM_GENERATED_FILE_WARNING@

#include <erm/utils/Version.h>

#include <refl.hpp>

#include <string_view>

namespace erm {
@ModuleDependenciesForwardDeclarations@
}

namespace erm {

class @ModuleName@
{
public:
	static constexpr std::string_view kModuleName{ "@MODULE_NAME@" };
	static constexpr std::string_view kModuleDescription{ "@MODULE_DESCRIPTION@" };
	static constexpr Version kModuleVersion{ @MODULE_VERSION_MAJOR@, @MODULE_VERSION_MINOR@, @MODULE_VERSION_PATCH@ };

private:
	class Impl
	{
	public:
		bool init() const;
		bool deinit() const;
		int run(int argc, char** argv) const;
	};

public:
	using DepsT = refl::type_list<@ModuleDependenciesList@>;
	static constexpr auto kDependencies = DepsT{};

	static bool initialized()
	{
		const auto& instance = getInstance();
		return instance.mInitialized;
	}

	static bool init()
	{
		auto& instance = getInstance();
		if (!instance.initialized())
		{
			instance.mInitialized = instance.mImpl.init();
		}
		return instance.initialized();
	}

	static bool deinit()
	{
		auto& instance = getInstance();
		if (instance.initialized())
		{
			instance.mInitialized = !instance.mImpl.deinit();
		}
		return !instance.initialized();
	}

	static int run(int argc, char** argv)
	{
		const auto& instance = getInstance();
		if (instance.initialized())
		{
			return instance.mImpl.run(argc, argv);
		}
		return EXIT_FAILURE;
	}

private:
	@ModuleName@() = default;

	static @ModuleName@& getInstance()
	{
		static @ModuleName@ sInstance;
		return sInstance;
	}

	Impl mImpl;
	bool mInitialized = false;

};

}

REFL_AUTO(
	type(erm::@ModuleName@)
);
