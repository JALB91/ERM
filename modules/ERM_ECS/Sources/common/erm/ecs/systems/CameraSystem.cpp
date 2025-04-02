#include "erm/ecs/systems/CameraSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/TransformSystem.h"

//#include "erm/rendering/window/Window.h"

#include <erm/input/Keys.h>
#include <erm/input/Mouse.h>

#include <erm/math/Types.h>

#include <erm/utils/Profiler.h>

#include <glm/gtc/matrix_transform.hpp>

namespace erm::ecs {

ERM_SYSTEM_IMPL(Camera)

CameraSystem::CameraSystem(ECS& ecs)
	: ISystem(ecs)
	, mTransformSystem(nullptr)
//	, mWindow(mEngine.getWindow())
{}

void CameraSystem::init()
{
	mTransformSystem = mECS.getSystem<TransformSystem>();
}

void CameraSystem::update(float dt)
{
	ERM_PROFILE_FUNCTION();

	for (ID i = 0; i < MAX_ID; ++i)
	{
		auto* cameraComp = getComponent(i);

		if (!cameraComp)
		{
			continue;
		}

		TransformComponent* transform = mTransformSystem->requireComponent(i);

		updateCameraComponent(*cameraComp, *transform, dt);
	}
}

void CameraSystem::postUpdate()
{
	ERM_PROFILE_FUNCTION();

	for (ID i = 0; i < MAX_ID; ++i)
	{
		auto* cameraComp = getComponent(i);

		if (!cameraComp)
		{
			continue;
		}

//		cameraComp->mProjectionMatrix = glm::perspective(
//			glm::radians(cameraComp->mFOV),
//			mWindow.getAspectRatio(),
//			cameraComp->mZNear,
//			cameraComp->mZFar);

#ifdef ERM_FLIP_PROJECTION
		cameraComp->mProjectionMatrix[1][1] *= -1.0f;
#endif

		cameraComp->setDirty(false);
	}
}

void CameraSystem::updateCameraComponent(CameraComponent& camera, TransformComponent& transform, float dt)
{
	(void)camera;
	(void)transform;
	(void)dt;
//	vec3 translation(0.0f);
//
//	if (mWindow.isMouseButtonDown(MOUSE_BUTTON_1))
//	{
//		vec3 rotation(
//			(mWindow.getPreviousMousePosY() - mWindow.getMousePosY()),
//			(mWindow.getPreviousMousePosX() - mWindow.getMousePosX()),
//			0.0f);
//		rotation = glm::radians(rotation * camera.mMouseSensibility) + transform.getRotation();
//
//		if (rotation.x > camera.mAngleLimit)
//		{
//			rotation.x = camera.mAngleLimit;
//		}
//		else if (rotation.x < -camera.mAngleLimit)
//		{
//			rotation.x = -camera.mAngleLimit;
//		}
//
//		transform.setRotation(rotation);
//	}
//	else if (mWindow.isMouseButtonDown(MOUSE_BUTTON_2))
//	{
//	}
//	else if (mWindow.isMouseButtonDown(MOUSE_BUTTON_3))
//	{
//	}
//
//	float movementSpeed = camera.mMovementSpeed;
//
//	if (mWindow.isKeyDown(KEY_LEFT_SHIFT))
//	{
//		movementSpeed *= 2.0f;
//	}
//	if (mWindow.isKeyDown(KEY_W))
//	{
//		translation.z -= movementSpeed * dt;
//	}
//	if (mWindow.isKeyDown(KEY_D))
//	{
//		translation.x += movementSpeed * dt;
//	}
//	if (mWindow.isKeyDown(KEY_S))
//	{
//		translation.z += movementSpeed * dt;
//	}
//	if (mWindow.isKeyDown(KEY_A))
//	{
//		translation.x -= movementSpeed * dt;
//	}
//	if (mWindow.isKeyDown(KEY_SPACE))
//	{
//		translation.y += movementSpeed * dt;
//	}
//	if (mWindow.isKeyDown(KEY_C))
//	{
//		translation.y -= movementSpeed * dt;
//	}
//
//	if (translation.x != 0.0f || translation.z != 0.0f || translation.y != 0.0f)
//	{
//		const auto& rotation = transform.getRotation();
//		mat4 rotationMatrix(glm::identity<mat4>());
//		rotationMatrix = glm::rotate(rotationMatrix, rotation.z, vec3(0.0f, 0.0f, 1.0f));
//		rotationMatrix = glm::rotate(rotationMatrix, rotation.y, vec3(0.0f, 1.0f, 0.0f));
//		rotationMatrix = glm::rotate(rotationMatrix, rotation.x, vec3(1.0f, 0.0f, 0.0f));
//
//		translation = rotationMatrix * vec4(translation, 1.0f);
//
//		transform.setTranslation(transform.getTranslation() + translation);
//	}
}

} // namespace erm::ecs
