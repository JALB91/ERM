#pragma once

#include "ec/IComponent.h"

#include "math/mat.h"

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
		
		inline const math::mat4& GetProjectionMatrix() const { return mProjectionMatrix; }
		
		void LookAt(const Entity& other);
		
	private:
		const IWindowSizeProvider& mWindowSizeProvider;
		const IMouseInfoProvider& mMouseInfoProvider;
		const IKeyInfoProvider& mKeyInfoProvider;
		TransformComponent& mTransform;
		math::mat4 mProjectionMatrix;
		
	};
	
}
