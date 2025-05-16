#include "erm/system/cmake/CMake.h"

#include "erm/system/cmake/CMakeBuildArgs.h"
#include "erm/system/cmake/CMakeGenerateArgs.h"
#include "erm/system/cmake/CMakeGeneratorData.h"
#include "erm/system/cmake/CMakeInstallArgs.h"
#include "erm/system/cmake/CMakeOpenArgs.h"
#include "erm/system/Utils.h"

#include <erm/log/Assert.h>

#include <magic_enum/magic_enum.hpp>

namespace erm::cmake::internal {

constexpr auto getCMakeBuildConfigName(CMakeBuildConfig logLevel)
{
	switch (logLevel)
	{
		case CMakeBuildConfig::DEBUG: return "Debug";
		case CMakeBuildConfig::RELEASE: return "Release";
		default: std::unreachable();
	}
}

}

namespace erm::cmake {

int generate(const CMakeGenerateArgs& args)
{
	std::string cmakeCommand = "cmake";
	
	for (const auto& [key, value] : args.mDefinitions)
	{
		cmakeCommand += std::format(" -D{}={}", key, value);
	}
	
	if (args.mGenerator.has_value())
	{
		const auto* data = getGeneratorDataFor(*args.mGenerator);
		if (data != nullptr)
		{
			cmakeCommand += std::format(" -G {}", data->mName);
		}
	}
	
	if (args.mLogLevel.has_value())
	{
		cmakeCommand += std::format(" --log-level={}", magic_enum::enum_name(*args.mLogLevel).data());
	}
	
	if (args.mGraphVizFilePath.has_value())
	{
		cmakeCommand += std::format(" --graphviz={}", args.mGraphVizFilePath->c_str());
	}
	
	cmakeCommand += std::format(" -B {} -S {}", args.mBuildPath.c_str(), args.mSourcePath.c_str());
	
	return std::system(cmakeCommand.c_str());
}

int install(const CMakeInstallArgs& args)
{
	std::string cmakeCommand = std::format("cmake --install {}", args.mBuildPath.c_str());
	
	if (args.mPrefix.has_value())
	{
		cmakeCommand += std::format(" --prefix {}", *args.mPrefix);
	}
	
	if (args.mBuildConfig.has_value())
	{
		cmakeCommand += std::format(" --config {}", internal::getCMakeBuildConfigName(*args.mBuildConfig));
	}
	
	if (args.mComponent.has_value())
	{
		cmakeCommand += std::format(" --component {}", *args.mComponent);
	}
	
	if (args.mVerbose)
	{
		cmakeCommand += " --verbose";
	}
	
	return std::system(cmakeCommand.c_str());
}

int build(const CMakeBuildArgs& args)
{
	std::string cmakeCommand = std::format("cmake --build {}", args.mBuildPath.c_str());
	
	if (args.mTarget.has_value())
	{
		cmakeCommand += std::format(" --target {}", *args.mTarget);
	}
	
	if (args.mBuildConfig.has_value())
	{
		cmakeCommand += std::format(" --config {}", internal::getCMakeBuildConfigName(*args.mBuildConfig));
	}
	
	if (args.mVerbose)
	{
		cmakeCommand += " --verbose";
	}
	
	if (args.mCleanFirst)
	{
		cmakeCommand += " --clean-first";
	}
	
	if (!args.mBuildToolArgs.empty())
	{
		cmakeCommand += " --";
		
		for (const auto& arg : args.mBuildToolArgs)
		{
			cmakeCommand += " " + arg;
		}
	}
	
	return std::system(cmakeCommand.c_str());
}

int open(const CMakeOpenArgs& args)
{
	std::string cmakeCommand = std::format("cmake --open {}", args.mBuildPath.c_str());
	return std::system(cmakeCommand.c_str());
}

}
