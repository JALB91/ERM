#pragma once

#include "erm/rendering/window/IWindowListener.h"

#include "erm/utils/Timer.h"
#include "erm/utils/FileLocator.h"

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
	public:
		Game();
		~Game();
		
		Game& operator=(const Game&) = delete;
		Game& operator=(Game&&) = delete;
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		
		bool Init();
		void Run();
		
		void OnUpdate(float dt);
		void OnPostUpdate();
		void OnPreRender();
		void OnImGuiRender();
		void OnRender();
		void OnPostRender();
		
		inline unsigned int GetFPS() const { return mFPS; }
		
		inline const Timer& GetTimer() const { return mTimer; }
		inline const FileLocator& GetFileLocator() const { return mFileLocator; }
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
		
		Timer mTimer;
		unsigned int mFPS;
		
		FileLocator mFileLocator;
		
		std::unique_ptr<Window> mWindow;
		std::unique_ptr<RenderContext> mRenderContext;
		std::unique_ptr<Renderer> mRenderer;
		std::unique_ptr<ResourcesManager> mResourcesManager;
		std::unique_ptr<ecs::ECS> mECS;
		
	};
	
}
