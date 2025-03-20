#include "erm/setup/Setup.h"

#include "erm/setup/CMakeGenerators.h"
#include "erm/setup/RenderingAPI.h"

#include <erm/fs/fs.h>

#include <erm/utils/assert/Assert.h>
#include <erm/utils/Utils.h>

#include <format>

namespace erm {

Setup::Setup()
	: mRenderingAPI(mArgsParser->addOptionalArg(
		std::nullopt,
		"rendering-api",
		ArgValueType::STRING,
		"VULKAN",
		"Set target rendering API"))
	, mVerbose(mArgsParser->addOptionalArg(
		'v',
		"verbose",
		ArgValueType::BOOLEAN,
		"false",
		"Enable a more verbose output"))
	, mOpen(mArgsParser->addOptionalArg(
		'o',
		"open",
		ArgValueType::BOOLEAN,
		"false",
		"Open the project after generating"))
	, mCompile(mArgsParser->addOptionalArg(
		'c', 
		"compile",
		ArgValueType::BOOLEAN,
		"false",
		"Compile the project after generating"))
	, mRTX(mArgsParser->addOptionalArg(
		std::nullopt,
		"rtx",
		ArgValueType::BOOLEAN,
		"false",
		"Enable ray tracing"))
	, mInteractive(mArgsParser->addOptionalArg(
		'i',
		"interactive",
		ArgValueType::BOOLEAN,
		"false",
		"Run cmake in interactive mode"))
	, mTracy(mArgsParser->addOptionalArg(
		std::nullopt,
		"tracy",
		ArgValueType::BOOLEAN,
		"false",
		"Enable tracy"))
	, mTrace(mArgsParser->addOptionalArg(
		std::nullopt,
		"trace",
		ArgValueType::BOOLEAN,
		"false",
		"Trace cmake output"))
	, mGenerator(mArgsParser->addOptionalArg(
		'G',
		"generator",
		ArgValueType::STRING,
		magic_enum::enum_name(getDefaultGeneratorFor(kHostPlatform)).data(),
		"Set the cmake generator to be used (if not specified, VS is used on Windows, XCode on OSX and Ninja on Linux)"))
{
	mRenderingAPI->setOptions<RenderingAPI>();
	mGenerator->setOptions<CMakeGenerator>();
	mArgsParser->setDescription("Setup ERM engine for local development");
	mArgsParser->setCallback([this](const erm::SubCommand& command) {
		return exec(command);
	});
}

CMakeGenerator Setup::getDefaultGeneratorFor(HostPlatform hostPlatform)
{
	switch (hostPlatform)
	{
		case HostPlatform::WINDOWS:
			return CMakeGenerator::VS;
		case HostPlatform::OSX:
			return CMakeGenerator::XCODE;
		case HostPlatform::LINUX:
		default:
			return CMakeGenerator::NINJA;
	}
}

const CMakeGeneratorData& Setup::getGeneratorDataFor(CMakeGenerator generator)
{
	const auto it = std::find_if(kAllGenerators.begin(), kAllGenerators.end(), [generator](const auto& generatorData) {
		return generatorData.mGenerator == generator;
	});
	ERM_ASSERT_HARD(it != kAllGenerators.end());
	return *it;
}

int Setup::exec(const SubCommand& /*command*/) const
{
	ERM_ASSERT_HARD(erm::utils::hasCommand("cmake"), "cmake required for setup");
	
	const auto renderingAPI = mRenderingAPI->get<std::string>();
	const auto verbose = mVerbose->get<bool>();
	const auto open = mOpen->get<bool>();
	const auto compile = mCompile->get<bool>();
	const auto rtx = mRTX->get<bool>();
	const auto interactive = mInteractive->get<bool>();
	const auto tracy = mTracy->get<bool>();
	const auto trace = mTrace->get<bool>();
	const auto generatorName = mGenerator->get<std::string>();
	
	auto generatorToUse = getDefaultGeneratorFor(kHostPlatform);
	
	if (!generatorName.empty())
	{
		const auto generator = magic_enum::enum_cast<CMakeGenerator>(generatorName);
		if (!generator.has_value())
		{
			ERM_LOG("The selected generator \"%s\" is not valid", generatorName.data());
			return EXIT_FAILURE;
		}
		generatorToUse = generator.value();
	}
	
	const auto& generatorData = getGeneratorDataFor(generatorToUse);
	
	if (!utils::contains(generatorData.mCompatibleHostPlatforms, kHostPlatform))
	{
		ERM_LOG("The selected generator \"%s\" is not available for this host platform", generatorData.mName.c_str());
		return EXIT_FAILURE;
	}
	
	const auto buildFolderName = std::format(
		"ERM_{}_{}_{}",
		magic_enum::enum_name(kHostPlatform),
		magic_enum::enum_name(generatorData.mGenerator),
		renderingAPI);
	const fs::path targetBuildPath = fs::getERMRoot() / "builds" / buildFolderName.data();
	
	if (!fs::exists(targetBuildPath))
	{
		fs::create_directory(targetBuildPath);
	}
	
	fs::current_path(targetBuildPath);
	
#if defined(ERM_OSX)
	ERM_ASSERT_HARD(!rtx, "RTX not supported on Mac");
#endif
	
	std::string cmakeCommand = interactive ? "ccmake" : "cmake";
	
	// Set ERM_RENDERING_API
	cmakeCommand += " -DERM_RENDERING_API=";
	cmakeCommand += renderingAPI.c_str();
	
	// Set ERM_RAY_TRACING_ENABLED
	cmakeCommand += " -DERM_RAY_TRACING_ENABLED=";
	cmakeCommand += rtx ? "ON" : "OFF";
	
	// Set ERM_TRACY_ENABLED
	cmakeCommand += " -DERM_TRACY_ENABLED=";
	cmakeCommand += tracy ? "ON" : "OFF";
	
	// Set CMakeGenerator
	cmakeCommand += " -G \"";
	cmakeCommand += generatorData.mName;
	cmakeCommand += "\"";
	
	if (verbose)
	{
		cmakeCommand += " --debug-output";
	}
	
	if (trace)
	{
		cmakeCommand += " --trace";
	}
	
#if defined(ERM_WINDOWS)
	cmakeCommand += " -A x64";
#endif
	
	cmakeCommand += " ../../";
	
	{
		const auto result = std::system(cmakeCommand.c_str());
		if (result != EXIT_SUCCESS)
		{
			return result;
		}
	}
	
	if (compile)
	{
		cmakeCommand = "cmake --build .";
		if (verbose)
		{
			cmakeCommand += "--debug-output";
		}
		if (trace)
		{
			cmakeCommand += "--trace";
		}
		
		const auto result = std::system(cmakeCommand.c_str());
		if (result != EXIT_SUCCESS)
		{
			return result;
		}
	}
	
	if (open)
	{
		cmakeCommand = "cmake --open .";
		if (verbose)
		{
			cmakeCommand += "--debug-output";
		}
		if (trace)
		{
			cmakeCommand += "--trace";
		}
		
		const auto result = std::system(cmakeCommand.c_str());
		if (result != EXIT_SUCCESS)
		{
			return result;
		}
	}
	
	return EXIT_SUCCESS;
}

}

