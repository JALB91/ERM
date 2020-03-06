#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/math/mat.h"

namespace {
	
	const float kMovementSpeed = 1.0f;
	const float kMouseSensibility = 0.25f;
	const float kAngleLimit = static_cast<float>(M_PI) * 0.35f;
	const float kFOV = 45.0f;
	const float kZNear = 0.1f;
	const float kZFar = 10000.0f;
	
}

namespace erm {
	namespace ecs {
		
		class CameraSystem;
		
		struct CameraComponent : public IComponent
		{
		public:
			typedef CameraSystem SYSTEM_TYPE;
			friend class CameraSystem;
			
			CameraComponent()
				: mMovementSpeed(1.0f)
				, mMouseSensibility(0.25f)
				, mAngleLimit(static_cast<float>(M_PI) * 0.35f)
				, mFOV(45.0f)
				, mZNear(0.1f)
				, mZFar(10000.0f)
			{}
			
			SENSIBLE_MEMBER(MovementSpeed, float, mMovementSpeed)
			SENSIBLE_MEMBER(MouseSensibility, float, mMouseSensibility)
			SENSIBLE_MEMBER(AngleLimit, float, mAngleLimit)
			SENSIBLE_MEMBER(FOV, float, mFOV)
			SENSIBLE_MEMBER(ZNear, float, mZNear)
			SENSIBLE_MEMBER(ZFar, float, mZFar)
			
			inline const math::mat4& GetProjectionMatrix() const { return mProjectionMatrix; }
			
		private:
			math::mat4 mProjectionMatrix;
			float mMovementSpeed;
			float mMouseSensibility;
			float mAngleLimit;
			float mFOV;
			float mZNear;
			float mZFar;
		
		};
		
	}
}

