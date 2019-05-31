#include "ec/components/CameraComponent.h"
#include "ec/components/TransformComponent.h"

#include "ec/Entity.h"

#include "interfaces/IWindowSizeProvider.h"
#include "interfaces/IMouseInfoProvider.h"
#include "interfaces/IKeyInfoProvider.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <imgui.h>

#include <GLFW/glfw3.h>

namespace {
	
	const float kMovementSpeed = 5.0f;
	
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
		
		if (mMouseInfoProvider.IsMouseButtonDown(GLFW_MOUSE_BUTTON_1))
		{
			glm::vec3 rotation (
				(mMouseInfoProvider.GetMousePosY() - mMouseInfoProvider.GetPreviousMousePosY()),
				(mMouseInfoProvider.GetMousePosX() - mMouseInfoProvider.GetPreviousMousePosX()),
				0.0f
			);
			mTransform.Rotate(glm::radians(rotation));
		}
		else if (mMouseInfoProvider.IsMouseButtonDown(GLFW_MOUSE_BUTTON_2))
		{
			
		}
		else if (mMouseInfoProvider.IsMouseButtonDown(GLFW_MOUSE_BUTTON_3))
		{
			
		}
		
		if (mKeyInfoProvider.IsKeyDown(GLFW_KEY_W))
		{
			translation.z += kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(GLFW_KEY_D))
		{
			translation.x += kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(GLFW_KEY_S))
		{
			translation.z -= kMovementSpeed;
		}
		if (mKeyInfoProvider.IsKeyDown(GLFW_KEY_A))
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
		mProjectionMatrix = glm::perspective(glm::radians(45.0f), mWindowSizeProvider.GetAspectRatio(), 0.1f, 1000.0f);
	}
	
	glm::mat4 CameraComponent::GetViewMatrix() const
	{
		return mProjectionMatrix * glm::inverse(mTransform.GetWorldTransform());
	}
	
	void CameraComponent::LookAt(const Entity& other)
	{
		
	}
	
}
