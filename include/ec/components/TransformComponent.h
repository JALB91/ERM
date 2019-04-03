#pragma once

#include "ec/IComponent.h"

#include "interfaces/ITransformable.h"

#include <glm/glm.hpp>

namespace erm {
	
	class TransformComponent:
		public IComponent,
		public ITransformable
	{
	public:
		TransformComponent(Entity& entity);
		
		// IComponent
		void OnUpdate(float dt) override;
		void OnPostUpdate() override;
		
		// ITransformable
		void SetTranslation(const glm::vec3& translation) override;
		void Translate(const glm::vec3& translation) override;
		
		void SetRotation(const glm::vec3& rotation) override;
		void Rotate(const glm::vec3& rotation) override;
		
		void SetScale(const glm::vec3& scale) override;
		void Scale(const glm::vec3& scale) override;
		
		inline const glm::mat4& GetWorldTransform() const { return mWorldTransform; }
		
	private:
		glm::mat4 mWorldTransform;
		
	};
	
}
