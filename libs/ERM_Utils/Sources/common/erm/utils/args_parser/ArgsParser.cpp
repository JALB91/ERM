#include "erm/utils/args_parser/ArgsParser.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/log/Log.h"
#include "erm/utils/Utils.h"

#include <erm/fs/fs.h>

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

	for (int i = 0; i < argc; ++i)
	{
		mArgs.emplace_back(argv[i]);
	}

	fs::path filePath = argv[0];
	if (ERM_EXPECT(fs::exists(filePath) && filePath.has_filename(), "The first argument should be the executable path"))
	{
		mArgs[0] = filePath.filename().string();
	}

	mMainCommand.setName(mArgs[0]);

	return mMainCommand.parse(std::span(mArgs.begin() + 1, mArgs.size() - 1));
}

}
