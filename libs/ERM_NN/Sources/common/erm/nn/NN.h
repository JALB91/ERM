#pragma once

#include "erm/nn/Parser.h"

#include <erm/utils/args_parser/ArgsParser.h>

namespace erm::nn {

class NN
{
public:
	NN();
	
	inline ArgsParser& getArgsParser()
	{
		return mArgsParser;
	}
	
private:
	int exec(const SubCommand& command);
	
	ArgsParser mArgsParser;
	OptionalArgHandle mStatementFiles;
	PositionalArgHandle mDataFile;
	PositionalArgHandle mOutputDir;
	
	Parser mNNParser;
};

}
