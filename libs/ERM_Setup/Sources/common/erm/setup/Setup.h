#pragma once

#include "erm/setup/CMakeGenerator.h"
#include "erm/setup/Compiler.h"
#include "erm/setup/HostPlatform.h"

#include <erm/utils/args_parser/ArgsParser.h>

namespace erm {
struct CMakeGeneratorData;
}

namespace erm {

class Setup
{
public:
	Setup();
	
	inline ArgsParser& getArgsParser()
	{
		return mArgsParser;
	}

private:
	static const CMakeGeneratorData& getGeneratorDataFor(CMakeGenerator generator);
	static CMakeGenerator getDefaultGeneratorFor(HostPlatform hostPlatform);
	
	static Compiler getDefaultCompilerFor(HostPlatform hostPlatform);
	
	int exec(const SubCommand& command) const;
	
	ArgsParser mArgsParser;
	OptionalArgHandle mRenderingAPI;
	OptionalArgHandle mVerbose;
	OptionalArgHandle mOpen;
	OptionalArgHandle mCompile;
	OptionalArgHandle mRTX;
	OptionalArgHandle mInteractive;
	OptionalArgHandle mTracy;
	OptionalArgHandle mTrace;
	OptionalArgHandle mGenerator;
	OptionalArgHandle mCompiler;

};

}
