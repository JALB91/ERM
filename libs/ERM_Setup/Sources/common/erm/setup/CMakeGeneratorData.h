#pragma once

#include "erm/setup/CMakeGenerator.h"
#include "erm/setup/HostPlatform.h"

#include <string>

namespace erm {

struct CMakeGeneratorData
{
	std::vector<HostPlatform> mCompatibleHostPlatforms;
	CMakeGenerator mGenerator;
	std::string mName;
};

}
