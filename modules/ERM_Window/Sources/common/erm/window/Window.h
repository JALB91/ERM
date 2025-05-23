#pragma once

#include "erm/window/IWindow.h"

#include <erm/modules_lib/IModuleObject.h>

#include <refl.hpp>

struct GLFWwindow;

namespace erm {

class Window : public IWindow, public IModuleObject<Window>
{
public:
	Window() noexcept;
	~Window() override;

	inline GLFWwindow* getGLFWWindow() const { return mGLFWWindow; }
	
	bool shouldClose();
	void update();
	void render();
	void postRender();

	// IWindow
	void onKey(int key, int scanCode, int action, int mods) override;
	void onMouseButton(int button, int action, int mods) override;
	void onMousePos(double xPos, double yPos) override;
	void onSizeChanged() override;
	void onMaximised(bool wasMaximised) override;
	void onFocusLost() override;
	void onFocus() override;
	BoundingBox2D getNormalizedViewport() const override;

private:
	void updateViewport();
	void updateAspectRatio();

	GLFWwindow* mGLFWWindow;
	
};

} // namespace erm

REFL_AUTO(
	type(erm::Window)
);
