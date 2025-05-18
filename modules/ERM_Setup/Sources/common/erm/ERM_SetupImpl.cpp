#include "erm/ERM_SetupImpl.h"

#include "erm/setup/Setup.h"

namespace erm {

bool ERM_SetupImpl::init() const
{
	return true;
}

bool ERM_SetupImpl::deinit() const
{
	return true;
}

int ERM_SetupImpl::run(int argc, char** argv) const
{
	erm::Setup setup;

	if (const auto* command = setup.getArgsParser().parseArgs(argc, argv); command != nullptr)
	{
		return command->execute();
	}

	return EXIT_FAILURE;
}

}
