#include "game/Game.h"

int main(int argc, char** argv)
{
	erm::Game game (640, 480);
	
	if (game.Init())
	{
		game.Run();
	}
	else
	{
		return -1;
	}
	
	return 0;
}
