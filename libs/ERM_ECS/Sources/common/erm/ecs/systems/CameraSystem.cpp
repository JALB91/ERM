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
//	, mWindow(mEngine.GetWindow())
{}

void CameraSystem::Init()
{
	mTransformSystem = mECS.GetSystem<TransformSystem>();
}

void CameraSystem::OnUpdate(float dt)
{
	ERM_PROFILE_FUNCTION();

	for (ID i = 0; i < MAX_ID; ++i)
	{
		CameraComponent* camera = GetComponent(i);

		if (!camera)
			continue;

		TransformComponent* transform = mTransformSystem->RequireComponent(i);

		UpdateCameraComponent(*camera, *transform, dt);
	}
}

void CameraSystem::OnPostUpdate()
{
	ERM_PROFILE_FUNCTION();

	for (ID i = 0; i < MAX_ID; ++i)
	{
		CameraComponent* camera = GetComponent(i);

		if (!camera)
			continue;

//		camera->mProjectionMatrix = glm::perspective(
//			glm::radians(camera->mFOV),
//			mWindow.GetAspectRatio(),
//			camera->mZNear,
//			camera->mZFar);

#ifdef ERM_FLIP_PROJECTION
		camera->mProjectionMatrix[1][1] *= -1.0f;
#endif

		camera->SetDirty(false);
	}
}

void CameraSystem::UpdateCameraComponent(CameraComponent& camera, TransformComponent& transform, float dt)
{
	(void)camera;
	(void)transform;
	(void)dt;
//	vec3 translation(0.0f);
//
//	if (mWindow.IsMouseButtonDown(MOUSE_BUTTON_1))
//	{
//		vec3 rotation(
//			(mWindow.GetPreviousMousePosY() - mWindow.GetMousePosY()),
//			(mWindow.GetPreviousMousePosX() - mWindow.GetMousePosX()),
//			0.0f);
//		rotation = glm::radians(rotation * camera.mMouseSensibility) + transform.GetRotation();
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
//		transform.SetRotation(rotation);
//	}
//	else if (mWindow.IsMouseButtonDown(MOUSE_BUTTON_2))
//	{
//	}
//	else if (mWindow.IsMouseButtonDown(MOUSE_BUTTON_3))
//	{
//	}
//
//	float movementSpeed = camera.mMovementSpeed;
//
//	if (mWindow.IsKeyDown(KEY_LEFT_SHIFT))
//	{
//		movementSpeed *= 2.0f;
//	}
//	if (mWindow.IsKeyDown(KEY_W))
//	{
//		translation.z -= movementSpeed * dt;
//	}
//	if (mWindow.IsKeyDown(KEY_D))
//	{
//		translation.x += movementSpeed * dt;
//	}
//	if (mWindow.IsKeyDown(KEY_S))
//	{
//		translation.z += movementSpeed * dt;
//	}
//	if (mWindow.IsKeyDown(KEY_A))
//	{
//		translation.x -= movementSpeed * dt;
//	}
//	if (mWindow.IsKeyDown(KEY_SPACE))
//	{
//		translation.y += movementSpeed * dt;
//	}
//	if (mWindow.IsKeyDown(KEY_C))
//	{
//		translation.y -= movementSpeed * dt;
//	}
//
//	if (translation.x != 0.0f || translation.z != 0.0f || translation.y != 0.0f)
//	{
//		const vec3& rotation = transform.GetRotation();
//		mat4 rotationMatrix(glm::identity<mat4>());
//		rotationMatrix = glm::rotate(rotationMatrix, rotation.z, vec3(0.0f, 0.0f, 1.0f));
//		rotationMatrix = glm::rotate(rotationMatrix, rotation.y, vec3(0.0f, 1.0f, 0.0f));
//		rotationMatrix = glm::rotate(rotationMatrix, rotation.x, vec3(1.0f, 0.0f, 0.0f));
//
//		translation = rotationMatrix * vec4(translation, 1.0f);
//
//		transform.SetTranslation(transform.GetTranslation() + translation);
//	}
}

} // namespace erm::ecs
