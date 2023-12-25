#pragma once

#include "erm/utils/args_parser/SubCommand.h"

#include <string>
#include <vector>

namespace erm {

class ArgsParser
{
public:
	ArgsParser();
	
	ArgsParser(const ArgsParser&) = delete;
	ArgsParser(ArgsParser&&) = delete;
	
	ArgsParser& operator=(const ArgsParser&) = delete;
	ArgsParser& operator=(ArgsParser&&) = delete;
	
	SubCommand* operator->();
	const SubCommand* parseArgs(int argc, char** argv);

private:
	std::vector<std::string> mArgs;
	SubCommand mMainCommand;

};

}
