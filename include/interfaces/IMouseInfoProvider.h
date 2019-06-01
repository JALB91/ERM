#pragma once

#include "input/Mouse.h"

namespace erm {
	
	class IMouseListener;
	
	class IMouseInfoProvider
	{
	public:
		virtual ~IMouseInfoProvider() = default;
		
		virtual void AddListener(IMouseListener& listener) = 0;
		virtual void RemoveListener(IMouseListener& listener) = 0;
		
		virtual double GetMousePosX() const = 0;
		virtual double GetMousePosY() const = 0;
		virtual double GetPreviousMousePosX() const = 0;
		virtual double GetPreviousMousePosY() const = 0;
		virtual bool IsMouseButtonDown(MouseButton mouseButton) const = 0;
		
	};
	
}
