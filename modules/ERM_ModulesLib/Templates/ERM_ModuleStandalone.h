#pragma once

@ERM_GENERATED_FILE_WARNING@

#include <erm/utils/Version.h>

#include <string_view>

namespace erm::@ModuleName@ {

static constexpr std::string_view kModuleName{ "@MODULE_NAME@" };
static constexpr std::string_view kModuleDescription{ "@MODULE_DESCRIPTION@" };
static constexpr Version kModuleVersion{ @MODULE_VERSION_MAJOR@, @MODULE_VERSION_MINOR@, @MODULE_VERSION_PATCH@ };

}
