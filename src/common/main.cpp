#include "erm/game/Game.h"

#include <iostream>

int main(int /*argc*/, char** /*argv*/)
{
	erm::Game game;
	
	if (game.Init())
	{
		try 
		{
			game.Run();
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
