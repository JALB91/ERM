#pragma once

#include "erm/system/cmake/CMakeGenerator.h"
#include "erm/system/HostPlatform.h"

#include <array>
#include <string>
#include <vector>

namespace erm {

struct CMakeGeneratorData
{
	std::vector<HostPlatform> mCompatibleHostPlatforms;
	CMakeGenerator mGenerator;
	std::string mName;
};

static const auto kUnixMKCMakeGeneratorData = CMakeGeneratorData {
	.mCompatibleHostPlatforms = { HostPlatform::OSX, HostPlatform::LINUX },
	.mGenerator = CMakeGenerator::UNIXMK,
	.mName = "Unix Makefiles",
};

static const auto kVSCMakeGeneratorData = CMakeGeneratorData {
	.mCompatibleHostPlatforms = { HostPlatform::WINDOWS },
	.mGenerator = CMakeGenerator::VS,
	.mName = "Visual Studio 17 2022",
};

static const auto kXcodeCMakeGeneratorData = CMakeGeneratorData {
	.mCompatibleHostPlatforms = { HostPlatform::OSX },
	.mGenerator = CMakeGenerator::XCODE,
	.mName = "Xcode",
};

static const auto kNinjaCMakeGeneratorData = CMakeGeneratorData {
	.mCompatibleHostPlatforms = { HostPlatform::WINDOWS, HostPlatform::OSX, HostPlatform::LINUX },
	.mGenerator = CMakeGenerator::NINJA,
	.mName = "Ninja",
};

static const std::array kAllGenerators {
	kUnixMKCMakeGeneratorData,
	kVSCMakeGeneratorData,
	kXcodeCMakeGeneratorData,
	kNinjaCMakeGeneratorData
};

const CMakeGeneratorData* getGeneratorDataFor(CMakeGenerator generator);

}
