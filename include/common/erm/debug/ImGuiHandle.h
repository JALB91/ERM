#pragma once

namespace erm {
	class Engine;
}

namespace erm {
	
	class ImGuiHandle
	{
	public:
		ImGuiHandle(Engine& engine);
		~ImGuiHandle();
		
		void OnRender();
		
	private:
		Engine& mEngine;
		
	};
	
}
