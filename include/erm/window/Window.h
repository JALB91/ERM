#pragma once

#include "erm/window/IWindow.h"

#include "erm/math/vec.h"

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
		
		bool Init();
		bool ShouldClose();
		void NewFrame();
		void Render();
		void PostRender();
		
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
	
}
