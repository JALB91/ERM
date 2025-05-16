#pragma once

@ERM_GENERATED_FILE_WARNING@

@AllModulesIncludes@

#include <refl.hpp>

namespace erm {

using AllModulesT = refl::type_list<@AllModulesList@>;

constexpr auto kAllModules = AllModulesT{};

}
