#pragma once

#include <erm/window/IWindowListener.h>
#include <erm/utils/Timer.h>
#include <erm/utils/UpdateManager.h>

#include <refl.hpp>

#include <vector>

namespace erm {
class AudioManager;
class Device;
class FileLocator;
class Renderer;
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
	inline const UpdateManager& getUpdateManager() const { return mUpdateManager; }

private:
	// IWindowListener
	void onKeyPressed(Key keyCode) override;
	void onKeyReleased(Key keyCode) override;
	void onMouseButtonPressed(MouseButton mouseButton) override;
	void onMouseButtonReleased(MouseButton mouseButton) override;
	void onMouseMoved(double xPos, double yPos) override;
	void onSizeChanged(u32 width, u32 height) override;
	void onFocusChanged() override;

	UpdateManager mUpdateManager;
	Timer mTimer;
	u16 mFPS;
	u16 mMaxFPS;
	double mTargetFrameTime;
	
	Window* mWindow;
	AudioManager* mAudioManager;
	Device* mDevice;
	Renderer* mRenderer;
	ecs::ECS* mECS;
	FileLocator* mFileLocator;

};

} // namespace erm

REFL_AUTO(type(erm::Engine));
