#include "erm/nn/NN.h"

#include "erm/nn/Program.h"

#include <erm/fs/fs.h>

namespace erm::nn {

NN::NN()
	: mDataFile(mArgsParser->addPositionalArg(
		"data",
		ArgValueType::STRING,
		"Path to the .nn file"))
	, mOutputDir(mArgsParser->addPositionalArg(
		"output",
		ArgValueType::STRING,
		"Path to the output directory for the generated files"))
{
	mArgsParser->setDescription("NN is a tool for code generation");
	mArgsParser->setCallback([this](const SubCommand& command) {
		return exec(command);
	});
}

int NN::exec(const SubCommand& /*command*/)
{
	const auto dataFile = mDataFile->get<std::string>();
	const auto outputDir = mOutputDir->get<std::string>();
	
	if (!fs::exists(outputDir) && !fs::create_directories(outputDir))
	{
		ERM_LOG_ERROR("Failed to create output directory %s", outputDir.c_str());
		return EXIT_FAILURE;
	}
	
	ERM_ASSERT_HARD(
		fs::exists(outputDir) && fs::is_directory(outputDir),
		"Invalid output directory \"%s\"",
		outputDir.c_str());
	ERM_ASSERT_HARD(
		fs::exists(dataFile) && fs::is_regular_file(dataFile),
		"Invalid input file \"%s\"",
		dataFile.c_str());
	
	auto program = Program(dataFile);
	
	if (!program.parse(utils::readFromFile(dataFile)))
	{
		return EXIT_FAILURE;
	}
	
	program.debugPrint();
	
	std::system(std::format(
		"touch {}/{}.cpp",
		outputDir,
		fs::path(dataFile).filename().replace_extension().c_str()).c_str());
	
	return EXIT_SUCCESS;
}

}
