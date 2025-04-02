#pragma once

#include <erm/input/Keys.h>
#include <erm/input/Mouse.h>

#include <erm/math/Types.h>

namespace erm {

class IWindowListener
{
public:
	virtual ~IWindowListener() = default;

	virtual void onSizeChanged(u32 width, u32 height) = 0;
	virtual void onKeyPressed(Key keyCode) = 0;
	virtual void onKeyReleased(Key keyCode) = 0;
	virtual void onMouseButtonPressed(MouseButton mouseButton) = 0;
	virtual void onMouseButtonReleased(MouseButton mouseButton) = 0;
	virtual void onMouseMoved(double xPos, double yPos) = 0;
	virtual void onFocusChanged() = 0;
};

} // namespace erm
