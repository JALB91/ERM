#pragma once

#include <erm/args_parser/ArgsParser.h>

namespace erm {
class FileLocator;
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
	int exec(const SubCommand& command) const;
	
	FileLocator& mFileLocator;
	ArgsParser mArgsParser;
	OptionalArgHandle mRenderingAPI;
	OptionalArgHandle mGenerator;
	OptionalArgHandle mCompiler;
	OptionalArgHandle mERMRootPath;
	OptionalArgHandle mOpen;
	OptionalArgHandle mCompile;
	OptionalArgHandle mRTX;
	OptionalArgHandle mTracy;
	OptionalArgHandle mLogLevel;
	OptionalArgHandle mUpdateBinaries;

};

}
