#pragma once

#include <erm/window/IWindowListener.h>

#include <erm/utils/FileLocator.h>
#include <erm/utils/Timer.h>

#include <memory>
#include <vector>

namespace erm {
class AudioManager;
class Device;
class Renderer;
class UpdateManager;
class Window;
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

	bool init();
	void run();

	void preUpdate();
	void update(float dt);
	void postUpdate();
	void preRender();
	void render();
	void postRender();

	inline u16 getFPS() const { return mFPS; }
	inline u16 getMaxFPS() const { return mMaxFPS; }

	void setMaxFPS(u16 maxFPS);

	inline const Timer& getTimer() const { return mTimer; }
	inline const FileLocator& getFileLocator() const { return mFileLocator; }
	inline AudioManager& getAudioManager() const { return *mAudioManager; }
	inline Window& getWindow() const { return *mWindow; }
	inline Device& getDevice() const { return *mDevice; }
	inline Renderer& getRenderer() const { return *mRenderer; }
	inline ecs::ECS& getECS() const { return *mECS; }
	inline UpdateManager& getUpdateManager() const { return *mUpdateManager; }

private:
	// IWindowListener
	void onKeyPressed(Key keyCode) override;
	void onKeyReleased(Key keyCode) override;
	void onMouseButtonPressed(MouseButton mouseButton) override;
	void onMouseButtonReleased(MouseButton mouseButton) override;
	void onMouseMoved(double xPos, double yPos) override;
	void onSizeChanged(u32 width, u32 height) override;
	void onFocusChanged() override;

	Timer mTimer;
	u16 mFPS;
	u16 mMaxFPS;
	double mTargetFrameTime;

	FileLocator mFileLocator;

	std::unique_ptr<Window> mWindow;
	std::unique_ptr<UpdateManager> mUpdateManager;
	std::unique_ptr<AudioManager> mAudioManager;
	std::unique_ptr<Device> mDevice;
	std::unique_ptr<Renderer> mRenderer;
	std::unique_ptr<ecs::ECS> mECS;
};

} // namespace erm
