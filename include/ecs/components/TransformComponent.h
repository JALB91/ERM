#pragma once

#include "ecs/ECSConfig.h"
#include "ecs/IComponent.h"

#include "math/vec.h"
#include "math/mat.h"

#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace erm {
	namespace ecs {
		
		class TransformSystem;
		
		struct TransformComponent : public IComponent
		{
		public:
			typedef TransformSystem SYSTEM_TYPE;
			friend class TransformSystem;
			
		public:
			TransformComponent()
				: IComponent()
				, mWorldTransform(glm::identity<math::mat4>())
				, mLocalTransform(glm::identity<math::mat4>())
				, mTranslation(0.0f)
				, mRotation(0.0f)
				, mScale(1.0f)
			{}
			
			SENSIBLE_MEMBER(WorldTransform, const math::mat4&, mWorldTransform)
			SENSIBLE_MEMBER(LocalTransform, const math::mat4&, mLocalTransform)
			SENSIBLE_MEMBER(Translation, const math::vec3&, mTranslation)
			SENSIBLE_MEMBER(Rotation, const math::vec3&, mRotation)
			SENSIBLE_MEMBER(Scale, const math::vec3&, mScale)
			
			inline math::vec3 GetTranslation() { return mTranslation; }
			
		private:
			math::mat4 mWorldTransform;
			math::mat4 mLocalTransform;
			math::vec3 mTranslation;
			math::vec3 mRotation;
			math::vec3 mScale;
			
		};
		
	}
}
