#include "ec/components/CameraComponent.h"
#include "ec/components/TransformComponent.h"

#include "ec/Entity.h"

#include "interfaces/IWindowSizeProvider.h"
#include "interfaces/IMouseInfoProvider.h"
#include "interfaces/IKeyInfoProvider.h"

#include "input/Keys.h"
#include "input/Mouse.h"

#include "math/vec.h"
#include "math/quat.h"

#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

namespace {
	
	const float kMovementSpeed = 1.0f;
	const float kMouseSensibility = 0.25f;
	const float kAngleLimit = static_cast<float>(M_PI) * 0.35f;
	const float kFOV = 45.0f;
	const float kZNear = 0.1f;
	const float kZFar = 10000.0f;
	
}

namespace erm {
	
	CameraComponent::CameraComponent(
		Entity& entity,
		const IWindowSizeProvider& windowSizeProvider,
		const IMouseInfoProvider& mouseInfoProvider,
		const IKeyInfoProvider& keyInfoProvider
		)
		: IComponent(entity)
		, mWindowSizeProvider(windowSizeProvider)
		, mMouseInfoProvider(mouseInfoProvider)
		, mKeyInfoProvider(keyInfoProvider)
		, mTransform(*entity.GetComponent<TransformComponent>())
		, mProjectionMatrix()
		, mMovementSpeed(kMovementSpeed)
		, mMouseSensibility(kMouseSensibility)
		, mAngleLimit(kAngleLimit)
		, mFOV(kFOV)
		, mZNear(kZNear)
		, mZFar(kZFar)
	{}
	
	void CameraComponent::OnUpdate(float dt)
	{
		math::vec4 translation (0.0f);
		
		if (!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemHovered())
		{
			if (mMouseInfoProvider.IsMouseButtonDown(MOUSE_BUTTON_1))
			{
				math::vec3 rotation (
					(mMouseInfoProvider.GetPreviousMousePosY() - mMouseInfoProvider.GetMousePosY()),
					(mMouseInfoProvider.GetPreviousMousePosX() - mMouseInfoProvider.GetMousePosX()),
					0.0f
				);
				rotation = glm::radians(rotation * kMouseSensibility) + mTransform.GetRotation();
				
				if (rotation.x > kAngleLimit)
				{
					rotation.x = kAngleLimit;
				}
				else if (rotation.x < -kAngleLimit)
				{
					rotation.x = -kAngleLimit;
				}
				
				mTransform.SetRotation(rotation);
			}
			else if (mMouseInfoProvider.IsMouseButtonDown(MOUSE_BUTTON_2))
			{
				
			}
			else if (mMouseInfoProvider.IsMouseButtonDown(MOUSE_BUTTON_3))
			{
				
			}
		}
		
		if (mKeyInfoProvider.IsKeyDown(KEY_W))
		{
			translation.z -= kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(KEY_D))
		{
			translation.x += kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(KEY_S))
		{
			translation.z += kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(KEY_A))
		{
			translation.x -= kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(KEY_SPACE))
		{
			translation.y += kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(KEY_LEFT_SHIFT))
		{
			translation.y -= kMovementSpeed;
		}
		
		if (translation.x != 0.0f || translation.z != 0.0f || translation.y != 0.0f)
		{
			math::mat4 rotationMatrix (glm::identity<math::mat4>());
			rotationMatrix = glm::rotate(rotationMatrix, mTransform.GetRotation().z, math::vec3(0.0f, 0.0f, 1.0f));
			rotationMatrix = glm::rotate(rotationMatrix, mTransform.GetRotation().y, math::vec3(0.0f, 1.0f, 0.0f));
			rotationMatrix = glm::rotate(rotationMatrix, mTransform.GetRotation().x, math::vec3(1.0f, 0.0f, 0.0f));
			
			mTransform.Translate(rotationMatrix * translation);
		}
	}
	
	void CameraComponent::OnPostUpdate()
	{
		mProjectionMatrix = glm::perspective(glm::radians(mFOV), mWindowSizeProvider.GetAspectRatio(), mZNear, mZFar);
	}
	
	void CameraComponent::LookAt(const Entity& other)
	{
		
	}
	
}
