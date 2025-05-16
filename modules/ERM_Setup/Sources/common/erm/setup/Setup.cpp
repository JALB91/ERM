#include "erm/setup/Setup.h"

#include <erm/fs/FileLocator.h>
#include <erm/fs/fs.h>
#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/system/cmake/CMake.h>
#include <erm/system/cmake/CMakeBuildArgs.h>
#include <erm/system/cmake/CMakeGenerateArgs.h>
#include <erm/system/cmake/CMakeInstallArgs.h>
#include <erm/system/cmake/CMakeOpenArgs.h>
#include <erm/system/compiler/Compiler.h>
#include <erm/system/gpu_device/RenderingAPI.h>
#include <erm/system/HostPlatform.h>

namespace erm {

Setup::Setup()
: mFileLocator(ObjectRegistry::get<FileLocator>())
, mRenderingAPI(mArgsParser->addOptionalArg(
	std::nullopt,
	"rendering-api",
	ArgValueType::STRING,
	magic_enum::enum_name(kDefaultRenderingAPI).data(),
	"Set target rendering API"))
, mGenerator(mArgsParser->addOptionalArg(
	'G',
	"generator",
	ArgValueType::STRING,
	magic_enum::enum_name(kDefaultCMakeGenerator).data(),
	"Set the cmake generator to be used (if not specified, VS is used on Windows, XCode on OSX and Ninja on Linux)"))
, mCompiler(mArgsParser->addOptionalArg(
	'C',
	"compiler",
	ArgValueType::STRING,
	magic_enum::enum_name(kDefaultCompiler).data(),
	"Set the compiler to be used (if not specified, MSVC is used on Windows, AppleClang on OSX and GNU on Linux"))
, mERMRootPath(mArgsParser->addOptionalArg(
	std::nullopt,
	"erm-root",
	ArgValueType::STRING,
	"",
	"Path to the ERM root directory"))
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
, mTracy(mArgsParser->addOptionalArg(
	std::nullopt,
	"tracy",
	ArgValueType::BOOLEAN,
	"false",
	"Enable tracy"))
, mLogLevel(mArgsParser->addOptionalArg(
	std::nullopt,
	"log-level",
	ArgValueType::STRING,
	magic_enum::enum_name(kDefaultCMakeLogLevel),
	"Set cmake log level"))
, mUpdateBinaries(mArgsParser->addOptionalArg(
	std::nullopt,
	"update-binaries",
	ArgValueType::BOOLEAN,
	"false",
	"Performs a release build and installs ERM binaries"))
{
	mRenderingAPI->setOptions<RenderingAPI>();
	mGenerator->setOptions<CMakeGenerator>();
	mCompiler->setOptions<Compiler>();
	mLogLevel->setOptions<CMakeLogLevel>();
	mArgsParser->setDescription("Setup ERM engine for local development");
	mArgsParser->setCallback([this](const SubCommand& command) {
		return exec(command);
	});
}

Setup::~Setup() = default;

int Setup::exec(const SubCommand& /*command*/) const
{
	const auto renderingAPI = mRenderingAPI->get<RenderingAPI>();
	const auto generator = mGenerator->get<CMakeGenerator>();
	const auto compiler = mCompiler->get<Compiler>();
	const auto open = mOpen->get<bool>();
	const auto compile = mCompile->get<bool>();
	const auto rtx = mRTX->get<bool>();
	const auto tracy = mTracy->get<bool>();
	const auto logLevel = mLogLevel->get<CMakeLogLevel>();
	const auto updateBinaries = mUpdateBinaries->get<bool>();
	
	if (const auto rootPath = mERMRootPath->get<fs::path>(); !rootPath.empty())
	{
		if (!mFileLocator->setERMRootPath(rootPath))
		{
			ERM_LOG_ERROR("Invalid path to ERM root: \"%s\"", rootPath.c_str());
			return EXIT_FAILURE;
		}
	}
	
	const auto ermRootPath = mFileLocator->getERMRootPath();
	
	const auto buildFolderName = std::format(
		"ERM_{}_{}_{}_{}",
		magic_enum::enum_name(kHostPlatform),
		magic_enum::enum_name(*generator),
		magic_enum::enum_name(*compiler),
		magic_enum::enum_name(*renderingAPI));
	const auto ermBuildPath = mFileLocator->getERMRootPath() / "builds" / buildFolderName.c_str();
	
	CMakeGenerateArgs generateArgs;
	generateArgs.mDefinitions.emplace_back("ERM_COMPILER", magic_enum::enum_name(*compiler));
	generateArgs.mDefinitions.emplace_back("ERM_RENDERING_API", magic_enum::enum_name(*renderingAPI));
	generateArgs.mDefinitions.emplace_back("ERM_TRACY_ENABLED", rtx ? "ON" : "OFF");
	generateArgs.mDefinitions.emplace_back("ERM_TRACY_ENABLED", tracy ? "ON" : "OFF");
	generateArgs.mGenerator = generator;
	generateArgs.mSourcePath = ermRootPath;
	generateArgs.mBuildPath = ermBuildPath;
	generateArgs.mLogLevel = logLevel;
	
	if (const auto result = cmake::generate(generateArgs); result != EXIT_SUCCESS)
	{
		return result;
	}
	
	if (compile)
	{
		const auto result = cmake::build(CMakeBuildArgs{
			.mBuildPath = ermBuildPath
		});
		
		if (result != EXIT_SUCCESS)
		{
			return result;
		}
	}
	
	if (open)
	{
		const auto result = cmake::open(CMakeOpenArgs{
			.mBuildPath = ermBuildPath
		});
		
		if (result != EXIT_SUCCESS)
		{
			return result;
		}
	}
	
	if (updateBinaries)
	{
		auto result = cmake::build(CMakeBuildArgs{
			.mBuildPath = ermBuildPath,
			.mBuildConfig = CMakeBuildConfig::RELEASE
		});
		
		if (result != EXIT_SUCCESS)
		{
			return result;
		}
		
		result = cmake::install(CMakeInstallArgs{
			.mComponent = "ERM",
			.mBuildPath = ermBuildPath,
			.mBuildConfig = CMakeBuildConfig::RELEASE
		});
		
		if (result != EXIT_SUCCESS)
		{
			return result;
		}
	}
	
	return EXIT_SUCCESS;
}

}

