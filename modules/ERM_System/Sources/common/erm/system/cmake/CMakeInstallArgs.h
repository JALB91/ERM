#pragma once

#include "erm/system/cmake/CMakeBuildConfig.h"

#include <erm/fs/fs.h>

#include <optional>
#include <string>

namespace erm {

struct CMakeInstallArgs
{
	fs::path mBuildPath;
	std::optional<std::string> mComponent;
	std::optional<std::string> mPrefix;
	std::optional<CMakeBuildConfig> mBuildConfig;
	bool mVerbose = false;
};

}
