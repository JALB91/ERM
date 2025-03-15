#include "erm/setup/Setup.h"

#include <erm/utils/log/Log.h>
#include <erm/utils/args_parser/ArgsParser.h>

int main(int argc, char** argv)
{
	erm::Setup setup;
	
	if (const auto* command = setup.getArgsParser().parseArgs(argc, argv); command != nullptr)
	{
		return command->execute();
	}
	
	return EXIT_FAILURE;
}
