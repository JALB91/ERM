#pragma once

#include "erm/utils/StaticString.h"
#include "erm/utils/args_parser/SubCommand.h"

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace erm {

class ArgsParser
{
public:
	ArgsParser(int argc, char** argv);

	SubCommand* parseArgs();

	SubCommand* operator->();
	SubCommand& addSubCommand(std::string_view name);

	void printHelp() const;

private:
	std::vector<str128> mArgs;

	std::unique_ptr<SubCommand> mMainCommand;
	std::vector<SubCommand> mSubCommands;

};

}