#pragma once

#include "ec/IComponent.h"

#include <glm/glm.hpp>

namespace erm {
	
	class Window;
	
	class DebugGameComponent : public IComponent
	{
	public:
		DebugGameComponent(Entity& entity);
		~DebugGameComponent();
		
		// IComponent
		void OnImGuiRender() override;
		
	private:
		const Window& mWindow;
		
	};
	
}
