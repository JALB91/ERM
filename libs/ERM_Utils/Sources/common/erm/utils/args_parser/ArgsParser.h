#pragma once

#include "erm/utils/args_parser/SubCommand.h"

#include <string>
#include <vector>

namespace erm {

class ArgsParser
{
public:
	ArgsParser();
	
	SubCommand* operator->();
	const SubCommand* parseArgs(int argc, char** argv);

private:
	std::vector<std::string> mArgs;
	SubCommand mMainCommand;

};

}
