#pragma once

#include "erm/input/Keys.h"
#include "erm/input/Mouse.h"

#include "erm/math/vec.h"

#include <set>

namespace erm {
	class IWindowListener;
}

namespace erm {

	class IWindow
	{
	public:
		IWindow();
		virtual ~IWindow() = default;

		void AddListener(IWindowListener& listener);
		void RemoveListener(IWindowListener& listener);

		virtual void OnKey(int key, int scanCode, int action, int mods) = 0;
		virtual void OnMouseButton(int button, int action, int mods) = 0;
		virtual void OnMousePos(double xPos, double yPos) = 0;
		virtual void OnSizeChanged(int width, int height) = 0;
		virtual void OnMaximised(bool wasMaximised) = 0;
		virtual void OnFocus() = 0;

		inline bool IsKeyDown(Key keyCode) const { return mPressedKeys.find(keyCode) != mPressedKeys.end(); }

		inline double GetMousePosX() const { return mMousePosX; }
		inline double GetMousePosY() const { return mMousePosY; }
		inline double GetPreviousMousePosX() const { return mPrevMousePosX; }
		inline double GetPreviousMousePosY() const { return mPrevMousePosY; }
		inline bool IsMouseButtonDown(MouseButton mouseButton) const { return mPressedButtons.find(mouseButton) != mPressedButtons.end(); }

		inline int GetWindowWidth() const { return mWindowWidth; }
		inline int GetWindowHeight() const { return mWindowHeight; }
		inline float GetAspectRatio() const { return mAspectRatio; }

		inline const math::vec2& GetViewport() const { return mViewport; }
		inline math::vec2 GetViewport() { return mViewport; }

	protected:
		std::set<IWindowListener*> mWindowListeners;
		std::set<Key> mPressedKeys;
		double mMousePosX, mPrevMousePosX;
		double mMousePosY, mPrevMousePosY;
		std::set<MouseButton> mPressedButtons;
		int mWindowWidth, mWindowHeight;
		math::vec2 mViewport;
		float mAspectRatio;
	};

} // namespace erm
