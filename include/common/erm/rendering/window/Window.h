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
		void OnUpdate();
		void OnRender();
		void OnPostRender();

		// IWindow
		void OnKey(int key, int scanCode, int action, int mods) override;
		void OnMouseButton(int button, int action, int mods) override;
		void OnMousePos(double xPos, double yPos) override;
		void OnSizeChanged() override;
		void OnMaximised(bool wasMaximised) override;
		void OnFocus() override;
		BoundingBox2D GetNormalizedViewport() const override;

	private:
		void UpdateViewport();
		void UpdateAspectRatio();

		GLFWwindow* mWindow;
	};

} // namespace erm
