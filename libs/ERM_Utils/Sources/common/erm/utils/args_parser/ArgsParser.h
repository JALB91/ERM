#pragma once

#include "erm/utils/StaticString.h"
#include "erm/utils/args_parser/SubCommand.h"

#include <vector>

namespace erm {

class ArgsParser
{
public:
	ArgsParser();
	
	SubCommand* parseArgs(int argc, char** argv);
	SubCommand* operator->();

private:
	std::vector<str64> mArgs;
	SubCommand mMainCommand;

};

}