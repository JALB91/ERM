#pragma once

#include "IKeyListener.h"
#include "IMouseListener.h"
#include "IWindowSizeListener.h"

#include <memory>

namespace erm {

	class Window;
	class Renderer;
	class IModelLoader;
	class ISceneObject;
	
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
		void OnRender();
		void OnPostRender();
		
		inline const Window& GetWindow() const { return *mWindow.get(); }
		inline const Renderer& GetRenderer() const { return *mRenderer.get(); }
		inline const IModelLoader& GetModelParser() const { return *mModelLoader.get(); }
		
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
		std::unique_ptr<IModelLoader> mModelLoader;
		
		std::unique_ptr<ISceneObject> mRoot;
		
		bool mIsDragging;
		
	};
	
}
