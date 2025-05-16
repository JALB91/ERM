#include "erm/ERM_Editor.h"

#include "erm/editor/ImGuiHandle.h"

#include <erm/engine/Engine.h>
#include <erm/modules_lib/ObjectRegistry.h>

#include <cstdlib>

namespace erm {

bool ERM_Editor::Impl::init() const
{
	return true;
}

bool ERM_Editor::Impl::deinit() const
{
	return true;
}

int ERM_Editor::Impl::run(int /*argc*/, char** /*argv*/) const
{
	auto engine = ObjectRegistry::get<Engine>();

	erm::ImGuiHandle handle(*engine);

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
