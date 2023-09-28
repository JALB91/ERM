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

private:
	std::vector<str128> mArgs;

	std::unique_ptr<SubCommand> mMainCommand;

};

}