#include "erm/ERM_EditorImpl.h"

#include "erm/editor/ImGuiHandle.h"

#include <erm/engine/Engine.h>
#include <erm/modules_lib/ObjectRegistry.h>

#include <cstdlib>

namespace erm {

bool ERM_EditorImpl::init()
{
	return true;
}

bool ERM_EditorImpl::deinit()
{
	return true;
}

int ERM_EditorImpl::run(int /*argc*/, char** /*argv*/)
{
	auto engine = ObjectRegistry::get<Engine>();

	erm::ImGuiHandle handle;

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
