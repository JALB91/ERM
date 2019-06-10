#pragma once

#include "input/Keys.h"
#include "input/Mouse.h"

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
	
}
