#include <erm/engine/Engine.h>

int main(int /*argc*/, char** /*argv*/)
{
	erm::Engine engine;

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
