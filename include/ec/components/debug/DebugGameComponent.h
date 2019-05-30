#pragma once

#include "ec/IComponent.h"

#include <glm/glm.hpp>

namespace erm {
	
	class RenderContext;
	
	class DebugGameComponent : public IComponent
	{
	public:
		DebugGameComponent(
			Entity& entity,
			const RenderContext& renderContext
		);
		~DebugGameComponent();
		
		// IComponent
		void OnImGuiRender() override;
		
	private:
		const RenderContext& mRenderContext;
		
	};
	
}
