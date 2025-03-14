#include "erm/setup/Setup.h"

#include "erm/setup/TargetAPI.h"

#include <erm/fs/fs.h>

#include <erm/utils/assert/Assert.h>
#include <erm/utils/Utils.h>

namespace erm {

Setup::Setup()
	: mTargetAPI(mArgsParser->addOptionalArg(
		't',
		"target-api",
		ArgValueType::STRING,
		"VULKAN",
		"Set target API"))
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
	, mUseGenerator(mArgsParser->addOptionalArg(
		std::nullopt,
		"use-generator",
		ArgValueType::BOOLEAN,
		"false",
		"Wheter or not an IDE generator should be used"))
{
	mTargetAPI->setOptions<TargetAPI>();
	mArgsParser->setDescription("Setup ERM engine for development");
	mArgsParser->setCallback([this](const erm::SubCommand& command) {
		return exec(command);
	});
}

int Setup::exec(const SubCommand& /*command*/) const
{
	ERM_ASSERT_HARD(erm::utils::hasCommand("cmake"), "cmake required for setup");
	
	const auto targetAPI = mTargetAPI->get<std::string>();
	const auto verbose = mVerbose->get<bool>();
	const auto open = mOpen->get<bool>();
	const auto compile = mCompile->get<bool>();
	const auto rtx = mRTX->get<bool>();
	const auto interactive = mInteractive->get<bool>();
	const auto tracy = mTracy->get<bool>();
	const auto trace = mTrace->get<bool>();
	const auto useGenerator = mUseGenerator->get<bool>();
	
	ERM_ASSERT_HARD(magic_enum::enum_cast<TargetAPI>(targetAPI) == TargetAPI::VULKAN, "Only Vulkan supported");
	
#if defined(ERM_WINDOWS)
	const auto buildFolderName = std::string("ERM_VS_") + targetAPI.data();
#elif defined(ERM_MAC)
	const auto buildFolderName = std::string("ERM_Xcode_") + targetAPI.data();
#endif
	const fs::path targetBuildPath = fs::getERMRoot() / "builds" / buildFolderName.data();
	
	if (!fs::exists(targetBuildPath))
	{
		fs::create_directory(targetBuildPath);
	}
	
	fs::current_path(targetBuildPath);
	
#if defined(ERM_MAC)
	ERM_ASSERT_HARD(!rtx, "RTX not supported on Mac");
#endif
	
	std::string cmakeCommand = interactive ? "ccmake" : "cmake";
	cmakeCommand += " -DERM_TARGET_API=";
	cmakeCommand += targetAPI.data();
	cmakeCommand += " -DERM_PLATFORM=";
	if (useGenerator)
	{
#if defined(ERM_WINDOWS)
		cmakeCommand += "Windows";
		cmakeCommand += " -G Visual Studio 17 2022";
#elif defined(ERM_MAC)
		cmakeCommand += "MacOS";
		cmakeCommand += " -G Xcode";
#endif
	}
	cmakeCommand += " -DERM_RAY_TRACING_ENABLED=";
	cmakeCommand += rtx ? "ON" : "OFF";
	cmakeCommand += " -DERM_TRACY_ENABLED=";
	cmakeCommand += tracy ? "ON" : "OFF";
	
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

