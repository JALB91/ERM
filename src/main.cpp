#include "game/Game.h"

int main(int /*argc*/, char** /*argv*/)
{
	erm::Game& game = erm::Game::GetInstance();
	
	if (game.Init())
	{
		game.Run();
	}
	else
	{
		erm::Game::DestroyInstance();
		return 1;
	}
	
	erm::Game::DestroyInstance();
	
	return 0;
}
