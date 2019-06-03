#pragma once

#include "ec/IComponent.h"

#include "interfaces/ITransformable.h"

#include "math/vec.h"
#include "math/mat.h"

namespace erm {
	
	class TransformComponent:
		public IComponent,
		public ITransformable
	{
	public:
		TransformComponent(Entity& entity);
		
		// IComponent
		void OnPostUpdate() override;
		
		// ITransformable
		void SetTranslation(const math::vec3& translation) override;
		void Translate(const math::vec3& translation) override;
		
		void SetRotation(const math::vec3& rotation) override;
		void Rotate(const math::vec3& rotation) override;
		
		void SetScale(const math::vec3& scale) override;
		void Scale(const math::vec3& scale) override;
		
		inline const math::mat4& GetWorldTransform() const { return mWorldTransform; }
		inline const math::mat4& GetLocalTransform() const { return mLocalTransform; }
		
	private:
		math::mat4 mWorldTransform;
		math::mat4 mLocalTransform;
		
	};
	
}
