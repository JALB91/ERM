@ERM_GENERATED_FILE_WARNING@

#include "erm/@ModuleName@.h"

#include <erm/modules_lib/ModulesManager.h>

int main(int argc, char** argv)
{
	auto modulesManager = erm::ModulesManager<erm::@ModuleName@>();

	if (!modulesManager.init())
	{
		return EXIT_FAILURE;
	}
    
	const auto result = modulesManager.run(argc, argv);

	if (!modulesManager.deinit())
	{
		return EXIT_FAILURE;
	}

	return result;
}