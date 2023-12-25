#include "erm/nn/NN.h"

#include <erm/fs/fs.h>

namespace erm::nn {

NN::NN()
	: mStatementFiles(mArgsParser->addOptionalArg(
		std::nullopt,
		"statements",
		ArgValueType::STRING,
		"",
		"List of the .nns files separated by ';'"))
	, mDataFile(mArgsParser->addPositionalArg(
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
	
	const auto& statementFilesStr = mStatementFiles->get<std::string>();
	const auto statementFiles = utils::splitString(statementFilesStr, ';');
	for (const auto& statementFile : statementFiles)
	{
		if (!mNNParser.parseStatements(utils::readFromFile(statementFile)))
		{
			ERM_LOG_ERROR("Error while parsing statements from \"%s\"", statementFile);
		}
	}
	
	ERM_ASSERT_HARD(
		fs::exists(outputDir) && fs::is_directory(outputDir),
		"Invalid output directory \"%s\"",
		outputDir.data());
	ERM_ASSERT_HARD(
		fs::exists(dataFile) && fs::is_regular_file(dataFile),
		"Invalid input file \"%s\"",
		dataFile.data());
	
	const auto program = mNNParser.parseProgram(dataFile, utils::readFromFile(dataFile));
	
	if (!program.has_value() || !program->validateTree())
	{
		return EXIT_FAILURE;
	}
	
	ERM_LOG(program->toJson().dump(4));
	
	return EXIT_SUCCESS;
}

}
