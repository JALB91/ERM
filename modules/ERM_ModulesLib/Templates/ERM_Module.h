#pragma once

@ERM_GENERATED_FILE_WARNING@

#include "erm/@MODULE_NAME@Impl.h"

#include <erm/stl/utils.h>
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

public:
	using DepsTypeListT = refl::type_list<@ModuleDependenciesList@>;
	static constexpr DepsTypeListT kDependenciesTypeList;

	using ObjectsTypeListT = @MODULE_NAME@ObjectsTypeListT;
	using ObjectsPackT = stl::pack_from_type_list_t<ObjectsTypeListT>;

public:
	@ModuleName@() noexcept;
	~@ModuleName@() = default;

	@ModuleName@(const @ModuleName@&) = delete;
	@ModuleName@(@ModuleName@&&) = delete;

	@ModuleName@& operator=(const @ModuleName@&) = delete;
	@ModuleName@& operator=(@ModuleName@&&) = delete;

	ObjectsPackT mObjects;

};

}

REFL_AUTO(
	type(erm::@ModuleName@)
);
