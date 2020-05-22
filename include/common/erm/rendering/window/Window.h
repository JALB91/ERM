#pragma once

#include "erm/rendering/window/IWindow.h"

#include "erm/math/vec.h"

#include <set>

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

		inline GLFWwindow* GetWindow() const { return mWindow; }

		bool Init();
		bool ShouldClose();
		void NewFrame();
		void OnRender();
		void OnPostRender();

		void OnKey(int key, int scanCode, int action, int mods) override;
		void OnMouseButton(int button, int action, int mods) override;
		void OnMousePos(double xPos, double yPos) override;
		void OnSizeChanged(int width, int height) override;
		void OnMaximised(bool wasMaximised) override;
		void OnFocus() override;

	private:
		void UpdateViewport();
		void UpdateAspectRatio();

		GLFWwindow* mWindow;
	};

} // namespace erm
