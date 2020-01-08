#pragma once

#include "window/IWindowListener.h"

#include <memory>

namespace erm {

	class Window;
	class RenderContext;
	class Renderer;
	
	namespace ecs {
		class ECS;
		struct Entity;
	}
	
	class Game : private IWindowListener
	{
	public:
		Game();
		~Game();
		
		bool Init();
		void Run();
		
		void OnUpdate(float dt);
		void OnPostUpdate();
		void OnPreRender();
		void OnImGuiRender();
		void OnRender();
		void OnPostRender();
		
		inline Window& GetWindow() const { return *mWindow; }
		inline RenderContext& GetRenderContext() const { return *mRenderContext; }
		inline Renderer& GetRenderer() const { return *mRenderer; }
		inline ecs::ECS& GetECS() const { return *mECS; }
		inline ecs::Entity& GetRoot() const { return *mRoot; }
		
	private:
		// IWindowListener
		void OnKeyPressed(Key keyCode) override;
		void OnKeyReleased(Key keyCode) override;
		void OnMouseButtonPressed(MouseButton mouseButton) override;
		void OnMouseButtonReleased(MouseButton mouseButton) override;
		void OnMouseMoved(double xPos, double yPos) override;
		void OnSizeChanged(int width, int height) override;
		
		std::unique_ptr<Window> mWindow;
		std::unique_ptr<RenderContext> mRenderContext;
		std::unique_ptr<Renderer> mRenderer;
		std::unique_ptr<ecs::ECS> mECS;
		
		ecs::Entity* mRoot;
		ecs::Entity* mCamera;
		ecs::Entity* mObject;
		
	};
	
}
