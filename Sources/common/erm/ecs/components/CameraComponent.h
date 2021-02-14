#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/math/mat.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm::ecs {
	class CameraSystem;
}

namespace erm::ecs {

	struct CameraComponent : public IComponent
	{
	public:
		typedef CameraSystem SYSTEM_TYPE;
		friend class CameraSystem;

	public:
		CameraComponent()
			: mProjectionMatrix(glm::identity<math::mat4>())
			, mMovementSpeed(10.0f)
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

} // namespace erm::ecs
