#pragma once

#include "ec/IComponent.h"

#include "math/mat.h"

namespace erm {
	
	class TransformComponent;
	class IWindow;
	
	class CameraComponent : public IComponent
	{
	public:
		CameraComponent(Entity& entity, const IWindow& window);
		
		// IComponent
		void OnUpdate(float dt) override;
		void OnPostUpdate() override;
		
		inline const math::mat4& GetProjectionMatrix() const { return mProjectionMatrix; }
		
		inline float GetMovementSpeed() const { return mMovementSpeed; }
		inline void SetMovementSpeed(float movementSpeed) { mMovementSpeed = movementSpeed; }
		
		inline float GetMouseSensibility() const { return mMouseSensibility; }
		inline void SetMouseSensibility(float mouseSensibility) { mMouseSensibility = mouseSensibility; }
		
		inline float GetAngleLimit() const { return mAngleLimit; }
		inline void SetAngleLimit(float angleLimit) { mAngleLimit = angleLimit; }
		
		inline float GetFOV() const { return mFOV; }
		inline void SetFOV(float fov) { mFOV = fov; }
		
		inline float GetZNear() const { return mZNear; }
		inline void SetZNear(float zNear) { mZNear = zNear; }
		
		inline float GetZFar() const { return mZFar; }
		inline void SetZFar(float zFar) { mZFar = zFar; }
		
		void LookAt(const Entity& other);
		
	private:
		const IWindow& mWindow;
		TransformComponent& mTransform;
		math::mat4 mProjectionMatrix;
		float mMovementSpeed;
		float mMouseSensibility;
		float mAngleLimit;
		float mFOV;
		float mZNear;
		float mZFar;
		
	};
	
}
