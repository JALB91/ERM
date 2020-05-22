#include "erm/engine/Engine.h"

#include <iostream>

int main(int /*argc*/, char** /*argv*/)
{
	erm::Engine engine;
	
	if (engine.Init())
	{
		try 
		{
			engine.Run();
		}
		catch (const std::exception& e) 
		{
			std::cerr << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	else
	{
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
