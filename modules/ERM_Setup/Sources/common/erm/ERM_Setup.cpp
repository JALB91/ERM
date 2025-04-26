#include "erm/ERM_Setup.h"

#include "erm/setup/Setup.h"

namespace erm {

bool ERM_Setup::Impl::init() const
{
	return true;
}

bool ERM_Setup::Impl::deinit() const
{
	return true;
}

int ERM_Setup::Impl::run(int argc, char** argv) const
{
	erm::Setup setup;

	if (const auto* command = setup.getArgsParser().parseArgs(argc, argv); command != nullptr)
	{
		return command->execute();
	}

	return EXIT_FAILURE;
}

}
