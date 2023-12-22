#pragma once

#include <erm/utils/args_parser/ArgsParser.h>

namespace erm {

class Setup
{
public:
	Setup();
	
	int exec(const SubCommand& command) const;
	
	inline ArgsParser& getArgsParser() 
	{
		return mArgsParser;
	}

private:
	ArgsParser mArgsParser;
	OptionalArgHandle mTargetAPI;
	OptionalArgHandle mVerbose;
	OptionalArgHandle mOpen;
	OptionalArgHandle mCompile;
	OptionalArgHandle mRTX;
	OptionalArgHandle mInteractive;
	OptionalArgHandle mTracy;
	OptionalArgHandle mTrace;

};

}
