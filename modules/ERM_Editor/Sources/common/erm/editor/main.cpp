#include "erm/editor/ImGuiHandle.h"

#include <erm/engine/Engine.h>

#include <cstdlib>

int main()
{
	erm::Engine engine;

	erm::ImGuiHandle handle(engine);

	if (engine.init())
	{
		engine.run();
	}
	else
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
