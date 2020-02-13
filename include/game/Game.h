#pragma once

#include "window/IWindowListener.h"

#include <memory>

namespace erm {

	class ResourcesManager;
	class Window;
	class RenderContext;
	class Renderer;
	
	namespace ecs {
		class ECS;
		struct Entity;
	}
	
	class Game : private IWindowListener
	{
	private:
		Game();
		
		static Game* mInstance;
		
	public:
		static Game& GetInstance();
		static void DestroyInstance();
		
		~Game();
		
		bool Init();
		void Run();
		
		void OnUpdate(float dt);
		void OnPostUpdate();
		void OnPreRender();
		void OnImGuiRender();
		void OnRender();
		void OnPostRender();
		
		inline ResourcesManager& GetResourcesManager() const { return *mResourcesManager; }
		inline Window& GetWindow() const { return *mWindow; }
		inline RenderContext& GetRenderContext() const { return *mRenderContext; }
		inline Renderer& GetRenderer() const { return *mRenderer; }
		inline ecs::ECS& GetECS() const { return *mECS; }
		
	private:
		// IWindowListener
		void OnKeyPressed(Key keyCode) override;
		void OnKeyReleased(Key keyCode) override;
		void OnMouseButtonPressed(MouseButton mouseButton) override;
		void OnMouseButtonReleased(MouseButton mouseButton) override;
		void OnMouseMoved(double xPos, double yPos) override;
		void OnSizeChanged(int width, int height) override;
		
		std::unique_ptr<ResourcesManager> mResourcesManager;
		std::unique_ptr<Window> mWindow;
		std::unique_ptr<RenderContext> mRenderContext;
		std::unique_ptr<Renderer> mRenderer;
		std::unique_ptr<ecs::ECS> mECS;
		
	};
	
}
