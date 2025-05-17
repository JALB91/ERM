#include "erm/args_parser/ArgsParser.h"

#include <erm/fs/fs.h>

#include <erm/log/Assert.h>
#include <erm/log/Log.h>

#include <erm/utils/Utils.h>

#include <algorithm>

namespace erm {

ArgsParser::ArgsParser()
{}

SubCommand* ArgsParser::operator->()
{
	return &mMainCommand;
}

SubCommand& ArgsParser::getMainCommand()
{
	return mMainCommand;
}

const SubCommand* ArgsParser::parseArgs(int argc, char** argv)
{
	ERM_ASSERT_HARD(argc > 0);
	mArgs.reserve(argc);
	
	const auto filePath = fs::path(argv[0]);
	ERM_ASSERT_HARD(filePath.has_filename(), "Expected executable filename as first argument");
	mMainCommand.setName(filePath.filename().string());

	for (int i = 0; i < argc; ++i)
	{
		mArgs.emplace_back(argv[i]);
	}

	return mMainCommand.parse(std::span(mArgs.begin() + 1, mArgs.size() - 1));
}

}
