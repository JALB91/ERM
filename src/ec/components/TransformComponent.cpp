#include "ec/components/TransformComponent.h"
#include "ec/Entity.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm {
	
	TransformComponent::TransformComponent(Entity& entity)
		: IComponent(entity)
		, ITransformable()
		, mWorldTransform(1.0f)
	{}
	
	void TransformComponent::OnPostUpdate()
	{
		if (mIsDirty)
		{
			mWorldTransform = glm::identity<glm::mat4>();
			
			if (Entity* parent = mEntity.GetParent())
			{
				mWorldTransform = parent->GetComponent<TransformComponent>()->GetWorldTransform();
			}
			
			mWorldTransform = glm::translate(mWorldTransform, mTranslation);
			mWorldTransform = glm::rotate(mWorldTransform, mRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
			mWorldTransform = glm::rotate(mWorldTransform, mRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
			mWorldTransform = glm::rotate(mWorldTransform, mRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
			mWorldTransform = glm::scale(mWorldTransform, mScale);
			
			mIsDirty = false;
		}
	}
	
	void TransformComponent::SetTranslation(const glm::vec3& translation)
	{
		if (translation == mTranslation) return;
		mTranslation = translation;
		mIsDirty = true;
	}
	
	void TransformComponent::Translate(const glm::vec3& translation)
	{
		if (translation == glm::vec3(0.0f)) return;
		mTranslation += translation;
		mIsDirty = true;
	}
	
	void TransformComponent::SetRotation(const glm::vec3& rotation)
	{
		if (rotation == mRotation) return;
		mRotation = rotation;
		mIsDirty = true;
	}
	
	void TransformComponent::Rotate(const glm::vec3& rotation)
	{
		if (rotation == glm::vec3(0.0f)) return;
		mRotation += rotation;
		mIsDirty = true;
	}
	
	void TransformComponent::SetScale(const glm::vec3& scale)
	{
		if (scale == mScale) return;
		mScale = scale;
		mIsDirty = true;
	}
	
	void TransformComponent::Scale(const glm::vec3& scale)
	{
		if (scale == glm::vec3(0.0f)) return;
		mScale += scale;
		mIsDirty = true;
	}
	
}
