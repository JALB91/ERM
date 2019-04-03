#pragma once

#include "interfaces/IKeyListener.h"
#include "interfaces/IMouseListener.h"
#include "interfaces/IWindowSizeListener.h"

#include <memory>

namespace erm {

	class Window;
	class Renderer;
	class Entity;
	
	class Game:
		private IKeyListener,
		private IMouseListener,
		private IWindowSizeListener
	{
	public:
		Game(int width, int height);
		~Game();
		
		bool Init();
		void Run();
		
		void OnUpdate(float dt);
		void OnPostUpdate();
		void OnPreRender();
		void OnImGuiRender();
		void OnRender();
		void OnPostRender();
		
		inline Window& GetWindow() const { return *mWindow.get(); }
		inline Renderer& GetRenderer() const { return *mRenderer.get(); }
		
	private:
		// IKeyListener
		void OnKeyPressed(int keyCode) override;
		void OnKeyReleased(int keyCode) override;
		
		// IMouseListener
		void OnMouseButtonPressed(int mouseButton) override;
		void OnMouseButtonReleased(int mouseButton) override;
		void OnMouseMoved(double xPos, double yPos) override;
		
		// IWindowSizeListener
		void OnSizeChanged(int width, int height) override;
		
		std::unique_ptr<Window> mWindow;
		std::unique_ptr<Renderer> mRenderer;
		
		std::unique_ptr<Entity> mRoot;
		
	};
	
}
