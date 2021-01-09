#include "erm/ecs/systems/CameraSystem.h"

#include "erm/ecs/ECS.h"
#include "erm/ecs/systems/TransformSystem.h"

#include "erm/rendering/window/IWindow.h"

#include "erm/input/Keys.h"
#include "erm/input/Mouse.h"

#include "erm/math/quat.h"
#include "erm/math/vec.h"

#include "erm/utils/Profiler.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm::ecs {

	CameraSystem::CameraSystem(ECS& ecs, const IWindow& window)
		: ISystem<CameraComponent>(ecs)
		, mWindow(window)
	{}

	void CameraSystem::Init()
	{
		mTransformSystem = &mECS.GetSystem<TransformSystem>();
	}

	void CameraSystem::OnUpdate(float dt)
	{
		PROFILE_FUNCTION();

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
		PROFILE_FUNCTION();

		for (ID i = 0; i < MAX_ID; ++i)
		{
			CameraComponent* camera = GetComponent(i);

			if (!camera)
				continue;

			camera->mProjectionMatrix = glm::perspective(
				glm::radians(camera->mFOV),
				mWindow.GetAspectRatio(),
				camera->mZNear,
				camera->mZFar);

#ifdef ERM_FLIP_PROJECTION
			camera->mProjectionMatrix[1][1] *= -1.0f;
#endif

			camera->SetDirty(false);
		}
	}

	void CameraSystem::UpdateCameraComponent(CameraComponent& camera, TransformComponent& transform, float dt)
	{
		math::vec3 translation(0.0f);

		if (mWindow.IsMouseButtonDown(MOUSE_BUTTON_1))
		{
			math::vec3 rotation(
				(mWindow.GetPreviousMousePosY() - mWindow.GetMousePosY()),
				(mWindow.GetPreviousMousePosX() - mWindow.GetMousePosX()),
				0.0f);
			rotation = glm::radians(rotation * camera.mMouseSensibility) + transform.mRotation;

			if (rotation.x > camera.mAngleLimit)
			{
				rotation.x = camera.mAngleLimit;
			}
			else if (rotation.x < -camera.mAngleLimit)
			{
				rotation.x = -camera.mAngleLimit;
			}

			transform.mRotation = rotation;
		}
		else if (mWindow.IsMouseButtonDown(MOUSE_BUTTON_2))
		{
		}
		else if (mWindow.IsMouseButtonDown(MOUSE_BUTTON_3))
		{
		}

		if (mWindow.IsKeyDown(KEY_W))
		{
			translation.z -= camera.mMovementSpeed * dt;
		}
		if (mWindow.IsKeyDown(KEY_D))
		{
			translation.x += camera.mMovementSpeed * dt;
		}
		if (mWindow.IsKeyDown(KEY_S))
		{
			translation.z += camera.mMovementSpeed * dt;
		}
		if (mWindow.IsKeyDown(KEY_A))
		{
			translation.x -= camera.mMovementSpeed * dt;
		}
		if (mWindow.IsKeyDown(KEY_SPACE))
		{
			translation.y += camera.mMovementSpeed * dt;
		}
		if (mWindow.IsKeyDown(KEY_LEFT_SHIFT))
		{
			translation.y -= camera.mMovementSpeed * dt;
		}

		if (translation.x != 0.0f || translation.z != 0.0f || translation.y != 0.0f)
		{
			math::mat4 rotationMatrix(glm::identity<math::mat4>());
			rotationMatrix = glm::rotate(rotationMatrix, transform.mRotation.z, math::vec3(0.0f, 0.0f, 1.0f));
			rotationMatrix = glm::rotate(rotationMatrix, transform.mRotation.y, math::vec3(0.0f, 1.0f, 0.0f));
			rotationMatrix = glm::rotate(rotationMatrix, transform.mRotation.x, math::vec3(1.0f, 0.0f, 0.0f));

			translation = rotationMatrix * math::vec4(translation, 1.0f);

			transform.mTranslation += translation;
		}
	}

} // namespace erm::ecs
