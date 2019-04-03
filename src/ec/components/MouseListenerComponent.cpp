#include "ec/components/MouseListenerComponent.h"
#include "ec/Entity.h"
#include "game/Game.h"
#include "window/Window.h"

namespace erm {
	
	MouseListenerComponent::MouseListenerComponent(Entity& entity)
		: IComponent(entity)
	{
		mEntity.GetGame().GetWindow().AddListener(*this);
	}
	
	MouseListenerComponent::~MouseListenerComponent()
	{
		mEntity.GetGame().GetWindow().RemoveListener(*this);
	}
	
	void MouseListenerComponent::OnMouseButtonPressed(int mouseButton)
	{}
	
	void MouseListenerComponent::OnMouseButtonReleased(int mouseButton)
	{}
	
	void MouseListenerComponent::OnMouseMoved(double xPos, double yPos)
	{}
	
}
