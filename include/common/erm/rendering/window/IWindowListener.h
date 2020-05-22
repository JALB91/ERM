#pragma once

#include "erm/input/Keys.h"
#include "erm/input/Mouse.h"

namespace erm {

	class IWindowListener
	{
	public:
		virtual ~IWindowListener() = default;

		virtual void OnSizeChanged(int width, int height) = 0;
		virtual void OnKeyPressed(Key keyCode) = 0;
		virtual void OnKeyReleased(Key keyCode) = 0;
		virtual void OnMouseButtonPressed(MouseButton mouseButton) = 0;
		virtual void OnMouseButtonReleased(MouseButton mouseButton) = 0;
		virtual void OnMouseMoved(double xPos, double yPos) = 0;
	};

} // namespace erm
