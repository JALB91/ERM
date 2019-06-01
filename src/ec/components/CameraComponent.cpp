#include "ec/components/CameraComponent.h"
#include "ec/components/TransformComponent.h"

#include "ec/Entity.h"

#include "interfaces/IWindowSizeProvider.h"
#include "interfaces/IMouseInfoProvider.h"
#include "interfaces/IKeyInfoProvider.h"

#include "input/Keys.h"
#include "input/Mouse.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <imgui.h>

namespace {
	
	const float kMovementSpeed = 5.0f;
	const float kMouseSensibility = 0.5f;
	const float kXAngleLimit = static_cast<float>(M_PI) * 0.25f;
	
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
	{}
	
	void CameraComponent::OnUpdate(float dt)
	{
		glm::vec4 translation (0.0f);
		
		if (!ImGui::IsAnyWindowHovered())
		{
			if (mMouseInfoProvider.IsMouseButtonDown(MOUSE_BUTTON_1))
			{
				glm::vec3 rotation (
					(mMouseInfoProvider.GetMousePosY() - mMouseInfoProvider.GetPreviousMousePosY()),
					(mMouseInfoProvider.GetMousePosX() - mMouseInfoProvider.GetPreviousMousePosX()),
					0.0f
				);
				rotation = glm::radians(rotation * kMouseSensibility) + mTransform.GetRotation();
				
				if (rotation.x > kXAngleLimit)
				{
					rotation.x = kXAngleLimit;
				}
				else if (rotation.x < -kXAngleLimit)
				{
					rotation.x = -kXAngleLimit;
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
			translation.z += kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(KEY_D))
		{
			translation.x += kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(KEY_S))
		{
			translation.z -= kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(KEY_A))
		{
			translation.x -= kMovementSpeed;
		}
		
		if (translation.x != 0.0f || translation.z != 0.0f)
		{
			glm::mat4 rotationMatrix (glm::identity<glm::mat4>());
			rotationMatrix = glm::rotate(rotationMatrix, mTransform.GetRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
			rotationMatrix = glm::rotate(rotationMatrix, mTransform.GetRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
			rotationMatrix = glm::rotate(rotationMatrix, mTransform.GetRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
			
			mTransform.Translate(rotationMatrix * translation);
		}
	}
	
	void CameraComponent::OnPostUpdate()
	{
		mProjectionMatrix = glm::perspective(glm::radians(45.0f), mWindowSizeProvider.GetAspectRatio(), 0.1f, 10000.0f);
	}
	
	glm::mat4 CameraComponent::GetViewMatrix() const
	{
		return mProjectionMatrix * glm::inverse(mTransform.GetWorldTransform());
	}
	
	void CameraComponent::LookAt(const Entity& other)
	{
		
	}
	
}
