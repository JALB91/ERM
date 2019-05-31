#pragma once

#include "ec/IComponent.h"

#include <glm/glm.hpp>

namespace erm {
	
	class TransformComponent;
	class IWindowSizeProvider;
	class IMouseInfoProvider;
	class IKeyInfoProvider;
	
	class CameraComponent : public IComponent
	{
	public:
		CameraComponent(
			Entity& entity,
			const IWindowSizeProvider& windowSizeProvider,
			const IMouseInfoProvider& mMouseInfoProvider,
			const IKeyInfoProvider& mKeyInfoProvider
		);
		
		// IComponent
		void OnUpdate(float dt) override;
		void OnPostUpdate() override;
		
		glm::mat4 GetViewMatrix() const;
		
		void LookAt(const Entity& other);
		
	private:
		const IWindowSizeProvider& mWindowSizeProvider;
		const IMouseInfoProvider& mMouseInfoProvider;
		const IKeyInfoProvider& mKeyInfoProvider;
		TransformComponent& mTransform;
		glm::mat4 mProjectionMatrix;
		
	};
	
}
