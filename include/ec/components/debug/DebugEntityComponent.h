#pragma once

#include "ec/IComponent.h"

namespace erm {
	
	class TransformComponent;
	
	class DebugEntityComponent: public IComponent
	{
	public:
		DebugEntityComponent(Entity& entity);
		~DebugEntityComponent();
		
		void OnImGuiRender() override;
		
	private:
		TransformComponent& mTransformComponent;
		
	};
	
}
