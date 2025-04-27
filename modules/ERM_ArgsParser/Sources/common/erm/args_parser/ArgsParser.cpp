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

const SubCommand* ArgsParser::parseArgs(int argc, char** argv)
{
	ERM_ASSERT_HARD(argc > 0);
	mArgs.reserve(argc);

	mArgs.emplace_back(fs::path(argv[0]).filename().string());

	for (int i = 1; i < argc; ++i)
	{
		mArgs.emplace_back(argv[i]);
	}

	mMainCommand.setName(mArgs[0]);

	return mMainCommand.parse(std::span(mArgs.begin() + 1, mArgs.size() - 1));
}

}
