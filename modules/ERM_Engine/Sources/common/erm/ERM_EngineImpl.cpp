#include "erm/ERM_Engine.h"

#include <erm/modules_lib/ObjectRegistry.h>

namespace erm {

bool ERM_EngineImpl::init()
{
	ObjectRegistry::set(mEngine);
	return true;
}

bool ERM_EngineImpl::deinit()
{
	ObjectRegistry::remove<Engine>();
	return true;
}

int ERM_EngineImpl::run(int /*argc*/, char** /*argv*/)
{
	auto engine = ObjectRegistry::get<Engine>();

	if (engine->init())
	{
		engine->run();
	}
	else
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

}
