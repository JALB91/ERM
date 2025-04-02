#pragma once

#include <erm/input/Keys.h>
#include <erm/input/Mouse.h>

#include <erm/math/BoundingBox.h>
#include <erm/math/Types.h>

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

	void addListener(IWindowListener& listener);
	void removeListener(IWindowListener& listener);

	virtual void onKey(int key, int scanCode, int action, int mods) = 0;
	virtual void onMouseButton(int button, int action, int mods) = 0;
	virtual void onMousePos(double xPos, double yPos) = 0;
	virtual void onSizeChanged() = 0;
	virtual void onMaximised(bool wasMaximised) = 0;
	virtual void onFocusLost();
	virtual void onFocus();

	inline bool hasFocus() const { return mHasFocus; }
	inline bool isKeyDown(Key keyCode) const { return mPressedKeys.find(keyCode) != mPressedKeys.end(); }

	inline double getMousePosX() const { return mMousePosX; }
	inline double getMousePosY() const { return mMousePosY; }
	inline double getPreviousMousePosX() const { return mPrevMousePosX; }
	inline double getPreviousMousePosY() const { return mPrevMousePosY; }
	inline bool isMouseButtonDown(MouseButton mouseButton) const { return mPressedButtons.find(mouseButton) != mPressedButtons.end(); }

	inline const ivec2& getFrameBufferSize() const { return mFrameBufferSize; }
	inline const ivec2& getWindowSize() const { return mWindowSize; }
	inline int getWindowWidth() const { return mWindowSize.x; }
	inline int getWindowHeight() const { return mWindowSize.y; }
	inline float getAspectRatio() const { return mAspectRatio; }

	virtual BoundingBox2D getNormalizedViewport() const = 0;
	inline const BoundingBox2D& getViewport() const { return mViewport; }
	inline BoundingBox2D getViewport() { return mViewport; }

protected:
	std::set<IWindowListener*> mWindowListeners;
	std::set<Key> mPressedKeys;
	double mMousePosX, mPrevMousePosX;
	double mMousePosY, mPrevMousePosY;
	std::set<MouseButton> mPressedButtons;
	ivec2 mFrameBufferSize;
	ivec2 mWindowSize;
	BoundingBox2D mViewport;
	float mAspectRatio;
	bool mHasFocus;
};

} // namespace erm
