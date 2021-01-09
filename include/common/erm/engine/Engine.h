#pragma once

#include "erm/rendering/window/IWindowListener.h"

#include "erm/utils/FileLocator.h"
#include "erm/utils/Timer.h"

#include <memory>

namespace erm {
	class ResourcesManager;
	class Window;
	class Device;
	class Renderer;
	class ImGuiHandle;
	namespace ecs {
		class ECS;
		struct Entity;
	} // namespace ecs
} // namespace erm

namespace erm {

	class Engine : private IWindowListener
	{
	public:
		Engine();
		~Engine();

		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;

		bool Init();
		void Run();

		void OnUpdate(float dt);
		void OnPostUpdate();
		void OnPreRender();
		void OnRender();
		void OnPostRender();

		inline unsigned int GetFPS() const { return mFPS; }
		inline unsigned int GetMaxFPS() const { return mMaxFPS; }

		inline void SetMaxFPS(unsigned int maxFPS) { mMaxFPS = maxFPS; }

		inline const Timer& GetTimer() const { return mTimer; }
		inline const FileLocator& GetFileLocator() const { return mFileLocator; }
		inline ResourcesManager& GetResourcesManager() const { return *mResourcesManager; }
		inline Window& GetWindow() const { return *mWindow; }
		inline Device& GetDevice() const { return *mDevice; }
		inline Renderer& GetRenderer() const { return *mRenderer; }
		inline ImGuiHandle& GetImGuiHandle() const { return *mImGuiHandle; }
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
		unsigned int mMaxFPS;

		FileLocator mFileLocator;

		std::unique_ptr<Window> mWindow;
		std::unique_ptr<Device> mDevice;
		std::unique_ptr<Renderer> mRenderer;
		std::unique_ptr<ImGuiHandle> mImGuiHandle;
		std::unique_ptr<ResourcesManager> mResourcesManager;
		std::unique_ptr<ecs::ECS> mECS;
	};

} // namespace erm
