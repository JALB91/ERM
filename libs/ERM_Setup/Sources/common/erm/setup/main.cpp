#include "erm/setup/Setup.h"

#include <erm/utils/log/Log.h>
#include <erm/utils/args_parser/ArgsParser.h>

int main(int argc, char** argv)
{
	erm::Setup setup;
	auto* command = setup.getArgsParser().parseArgs(argc, argv);
	if (command != nullptr)
	{
		return command->callback();
	}
	
	return EXIT_SUCCESS;
}
