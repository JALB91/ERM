#pragma once

#include "erm/system/cmake/CMakeDefinition.h"
#include "erm/system/cmake/CMakeLogLevel.h"
#include "erm/system/cmake/CMakeGenerator.h"

#include <erm/fs/fs.h>

#include <optional>
#include <vector>

namespace erm {

struct CMakeGenerateArgs
{
	fs::path mBuildPath;
	fs::path mSourcePath;
	std::vector<CMakeDefinition> mDefinitions;
	std::optional<CMakeGenerator> mGenerator;
	std::optional<CMakeLogLevel> mLogLevel;
	std::optional<fs::path> mGraphVizFilePath;
};

}
