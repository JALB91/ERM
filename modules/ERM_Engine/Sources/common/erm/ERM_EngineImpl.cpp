#include "erm/ERM_Engine.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

int ERM_Engine::run(int /*argc*/, char** /*argv*/)
{
	auto& engine = ObjectRegistry::require<Engine>();
	engine.run();

	return EXIT_SUCCESS;
}

}
