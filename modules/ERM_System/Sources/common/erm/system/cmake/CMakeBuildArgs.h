#pragma once

#include "erm/system/cmake/CMakeBuildConfig.h"

#include <erm/fs/fs.h>

#include <optional>
#include <string>
#include <vector>

namespace erm {

struct CMakeBuildArgs
{
	fs::path mBuildPath;
	std::vector<std::string> mBuildToolArgs;
	std::optional<std::string> mTarget;
	std::optional<CMakeBuildConfig> mBuildConfig;
	bool mCleanFirst = false;
	bool mVerbose = false;
};

}
