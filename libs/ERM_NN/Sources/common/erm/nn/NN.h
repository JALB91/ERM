#pragma once

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
	PositionalArgHandle mDataFile;
	PositionalArgHandle mOutputDir;
	
};

}
