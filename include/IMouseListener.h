#pragma once

namespace erm {
	
	class IMouseListener
	{
	public:
		virtual ~IMouseListener() = default;
		
		virtual void OnMouseButtonPressed(int mouseButton) = 0;
		virtual void OnMouseButtonReleased(int mouseButton) = 0;
		virtual void OnMouseMoved(double xPos, double yPos) = 0;
		
	};
	
}
