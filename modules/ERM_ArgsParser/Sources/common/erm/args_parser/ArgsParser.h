#pragma once

#include "erm/args_parser/SubCommand.h"

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
	SubCommand& getMainCommand();
	const SubCommand* parseArgs(int argc, char** argv);
	
	inline const std::vector<std::string>& getArgs() const
	{
		return mArgs;
	}

private:
	std::vector<std::string> mArgs;
	SubCommand mMainCommand;

};

}
