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
	const std::span cmdArgs(mArgs.begin() + 1, mArgs.size() - 1);
	for (size_t index = 0; index < cmdArgs.size(); ++index)
	{
		const auto& arg = cmdArgs[index];

		if (!arg.startsWith('-'))
		{
			for (auto& cmd : mSubCommands)
			{
				if (cmd.getName() == arg)
				{
					return cmd.parse(std::span(cmdArgs.begin() + index + 1, cmdArgs.size() - index - 1)) ? &cmd : nullptr;
				}
			}
		}
	}

	return mMainCommand->parse(cmdArgs) ? mMainCommand.get() : nullptr;
}

SubCommand* ArgsParser::operator->()
{
	return mMainCommand.get();
}

SubCommand& ArgsParser::addSubCommand(std::string_view name)
{
	ERM_ASSERT_HARD(!name.empty());
	return mSubCommands.emplace_back(name);
}

void ArgsParser::printHelp() const
{
	str256 logStr;
	logStr.format("Usage:\n\t%s", mMainCommand->getName());

	for (size_t i = 0; i < mSubCommands.size(); ++i)
	{
		if (i == 0)
		{
			logStr += " <";
		}
		logStr += mSubCommands[i].getName();
		if (i == mSubCommands.size() - 1)
		{
			logStr += '>';
		}
		else
		{
			logStr += '|';
		}
	}

	ERM_LOG(logStr);
}

}