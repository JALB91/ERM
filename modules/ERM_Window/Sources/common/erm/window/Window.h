#pragma once

#include "erm/window/IWindow.h"

#include <refl.hpp>

struct GLFWwindow;

namespace erm {

class Window : public IWindow
{
public:
	Window();
	~Window();

	Window(const Window&) = delete;
	Window(Window&&) = delete;

	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;

	inline GLFWwindow* getWindow() const { return mWindow; }

	bool init();
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

	GLFWwindow* mWindow;
};

} // namespace erm

REFL_AUTO(
	type(erm::Window)
);
