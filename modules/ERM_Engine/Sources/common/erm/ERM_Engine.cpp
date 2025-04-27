#include "erm/ERM_Engine.h"

#include "erm/engine/Engine.h"

#include <erm/modules/ObjectRegistry.h>

namespace erm {

bool ERM_Engine::Impl::init() const
{
	ObjectRegistry::set(std::make_shared<Engine>());
	return true;
}

bool ERM_Engine::Impl::deinit() const
{
	ObjectRegistry::remove<Engine>();
	return true;
}

int ERM_Engine::Impl::run(int argc, char** argv) const
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
