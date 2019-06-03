#include "ec/components/TransformComponent.h"
#include "ec/Entity.h"

#include <glm/gtc/matrix_transform.hpp>

namespace erm {
	
	TransformComponent::TransformComponent(Entity& entity)
		: IComponent(entity)
		, ITransformable()
		, mWorldTransform(glm::identity<math::mat4>())
		, mLocalTransform(glm::identity<math::mat4>())
	{}
	
	void TransformComponent::OnPostUpdate()
	{
		if (mIsDirty)
		{
			mWorldTransform = glm::identity<math::mat4>();
			mLocalTransform = glm::identity<math::mat4>();
			
			if (Entity* parent = mEntity.GetParent())
			{
				mWorldTransform = parent->GetComponent<TransformComponent>()->GetWorldTransform();
			}
			
			mLocalTransform = glm::translate(mLocalTransform, mTranslation);
			mLocalTransform = glm::rotate(mLocalTransform, mRotation.z, math::vec3(0.0f, 0.0f, 1.0f));
			mLocalTransform = glm::rotate(mLocalTransform, mRotation.y, math::vec3(0.0f, 1.0f, 0.0f));
			mLocalTransform = glm::rotate(mLocalTransform, mRotation.x, math::vec3(1.0f, 0.0f, 0.0f));
			mLocalTransform = glm::scale(mLocalTransform, mScale);
			
			mWorldTransform *= mLocalTransform;
			
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
