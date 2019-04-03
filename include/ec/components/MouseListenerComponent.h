#pragma once

#include "ec/IComponent.h"

#include "interfaces/IMouseListener.h"

namespace erm {
	
	class MouseListenerComponent:
		public IComponent,
		private IMouseListener
	{
	public:
		MouseListenerComponent(Entity& entity);
		~MouseListenerComponent();
		
		inline bool IsDragging() const { return mIsDragging; }
		
	private:
		// IMouseListener
		void OnMouseButtonPressed(int mouseButton) override;
		void OnMouseButtonReleased(int mouseButton) override;
		void OnMouseMoved(double xPos, double yPos) override;
		
		bool mIsDragging;
		
	};
	
}
