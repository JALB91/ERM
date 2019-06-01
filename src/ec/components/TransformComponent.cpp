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
			mWorldTransform = glm::identity<math::mat4>();
			
			if (Entity* parent = mEntity.GetParent())
			{
				mWorldTransform = parent->GetComponent<TransformComponent>()->GetWorldTransform();
			}
			
			mWorldTransform = glm::translate(mWorldTransform, mTranslation);
			mWorldTransform = glm::rotate(mWorldTransform, mRotation.z, math::vec3(0.0f, 0.0f, 1.0f));
			mWorldTransform = glm::rotate(mWorldTransform, mRotation.y, math::vec3(0.0f, 1.0f, 0.0f));
			mWorldTransform = glm::rotate(mWorldTransform, mRotation.x, math::vec3(1.0f, 0.0f, 0.0f));
			mWorldTransform = glm::scale(mWorldTransform, mScale);
			
			mIsDirty = false;
		}
	}
	
	void TransformComponent::SetTranslation(const math::vec3& translation)
	{
		if (translation == mTranslation) return;
		mTranslation = translation;
		mIsDirty = true;
	}
	
	void TransformComponent::Translate(const math::vec3& translation)
	{
		if (translation == math::vec3(0.0f)) return;
		mTranslation += translation;
		mIsDirty = true;
	}
	
	void TransformComponent::SetRotation(const math::vec3& rotation)
	{
		if (rotation == mRotation) return;
		mRotation = rotation;
		mIsDirty = true;
	}
	
	void TransformComponent::Rotate(const math::vec3& rotation)
	{
		if (rotation == math::vec3(0.0f)) return;
		mRotation += rotation;
		mIsDirty = true;
	}
	
	void TransformComponent::SetScale(const math::vec3& scale)
	{
		if (scale == mScale) return;
		mScale = scale;
		mIsDirty = true;
	}
	
	void TransformComponent::Scale(const math::vec3& scale)
	{
		if (scale == math::vec3(0.0f)) return;
		mScale += scale;
		mIsDirty = true;
	}
	
}
