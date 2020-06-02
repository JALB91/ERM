#pragma once

#include "erm/input/Keys.h"
#include "erm/input/Mouse.h"

#include "erm/math/BoundingBox.h"

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
		virtual void OnSizeChanged() = 0;
		virtual void OnMaximised(bool wasMaximised) = 0;
		virtual void OnFocus() = 0;

		inline bool IsKeyDown(Key keyCode) const { return mPressedKeys.find(keyCode) != mPressedKeys.end(); }

		inline double GetMousePosX() const { return mMousePosX; }
		inline double GetMousePosY() const { return mMousePosY; }
		inline double GetPreviousMousePosX() const { return mPrevMousePosX; }
		inline double GetPreviousMousePosY() const { return mPrevMousePosY; }
		inline bool IsMouseButtonDown(MouseButton mouseButton) const { return mPressedButtons.find(mouseButton) != mPressedButtons.end(); }

		inline const math::vec<2, int>& GetFrameBufferSize() const { return mFrameBufferSize; }
		inline const math::vec<2, int>& GetWindowSize() const { return mWindowSize; }
		inline int GetWindowWidth() const { return mWindowSize.x; }
		inline int GetWindowHeight() const { return mWindowSize.y; }
		inline float GetAspectRatio() const { return mAspectRatio; }

		virtual BoundingBox2D GetNormalizedViewport() const = 0;
		inline const BoundingBox2D& GetViewport() const { return mViewport; }
		inline BoundingBox2D GetViewport() { return mViewport; }

	protected:
		std::set<IWindowListener*> mWindowListeners;
		std::set<Key> mPressedKeys;
		double mMousePosX, mPrevMousePosX;
		double mMousePosY, mPrevMousePosY;
		std::set<MouseButton> mPressedButtons;
		math::vec<2, int> mFrameBufferSize;
		math::vec<2, int> mWindowSize;
		BoundingBox2D mViewport;
		float mAspectRatio;
	};

} // namespace erm
