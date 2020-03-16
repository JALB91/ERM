#pragma once

namespace erm {
	
	class Game;
	
	class ImGuiHandle
	{
	public:
		ImGuiHandle(Game& game);
		~ImGuiHandle();
		
		void OnRender();
		
	private:
		Game& mGame;
		
	};
	
}
