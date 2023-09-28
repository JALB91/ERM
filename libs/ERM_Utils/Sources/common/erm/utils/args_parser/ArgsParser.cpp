#include "erm/utils/args_parser/ArgsParser.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/log/Log.h"
#include "erm/utils/Utils.h"

#include <algorithm>
#include <filesystem>

namespace erm {

ArgsParser::ArgsParser(int argc, char** argv)
{
	ERM_ASSERT_HARD(argc > 0);
	mArgs.reserve(argc);

	for (int i = 0; i < argc; ++i)
	{
		mArgs.emplace_back(argv[i]);
	}

	std::filesystem::path filePath = argv[0];
	if (ERM_EXPECT_DESCR(std::filesystem::exists(filePath) && filePath.has_filename(), "The first argument should be the executable name"))
	{
		mArgs[0] = filePath.filename().string();
	}

	mMainCommand = std::make_unique<SubCommand>(mArgs[0]);
}

SubCommand* ArgsParser::parseArgs()
{
	return mMainCommand->parse(std::span(mArgs.begin() + 1, mArgs.size() -1));
}

SubCommand* ArgsParser::operator->()
{
	return mMainCommand.get();
}

}