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
	SENSIBLE_MEMBER(MovementSpeed, float, 10.0f)
	SENSIBLE_MEMBER(MouseSensibility, float, 0.5f)
	SENSIBLE_MEMBER(AngleLimit, float, static_cast<float>(M_PI) * 0.35f)
	SENSIBLE_MEMBER(FOV, float, 45.0f)
	SENSIBLE_MEMBER(ZNear, float, 0.1f)
	SENSIBLE_MEMBER(ZFar, float, 10000.0f)

	inline const math::mat4& GetProjectionMatrix() const { return mProjectionMatrix; }

private:
	math::mat4 mProjectionMatrix = glm::identity<math::mat4>();
};

} // namespace erm::ecs
