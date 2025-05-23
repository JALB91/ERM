#include "erm/ERM_SetupImpl.h"

#include "erm/ERM_Setup.h"

#include "erm/setup/Setup.h"

namespace erm {

int ERM_Setup::run(int argc, char** argv)
{
	erm::Setup setup;

	if (const auto* command = setup.getArgsParser().parseArgs(argc, argv); command != nullptr)
	{
		return command->execute();
	}

	return EXIT_FAILURE;
}

}
