#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace erm {
	
	class ITransformable
	{
	public:
		inline ITransformable(
			const glm::vec3& translation,
			const glm::vec3& rotation,
			const glm::vec3& scale
		)
			: mTranslation(translation)
			, mRotation(rotation)
			, mScale(scale)
		{}
		inline ITransformable()
		: ITransformable(
			glm::vec3(0.0f),
			glm::vec3(0.0f),
			glm::vec3(1.0f)
		)
		{}
		virtual ~ITransformable() = default;
		
		inline glm::vec3 GetTranslation() { return mTranslation; }
		inline const glm::vec3& GetTranslation() const { return mTranslation; }
		virtual inline void SetTranslation(const glm::vec3& translation) { mTranslation = translation; }
		virtual inline void Translate(const glm::vec3& translation) { mTranslation += translation; }
		
		inline glm::vec3 GetRotation() { return mRotation; }
		inline const glm::vec3& GetRotation() const { return mRotation; }
		virtual inline void SetRotation(const glm::vec3& rotation) { mRotation = rotation; }
		virtual inline void Rotate(const glm::vec3& rotation) { mRotation += rotation; }
		
		inline glm::vec3 GetScale() { return mScale; }
		inline const glm::vec3& GetScale() const { return mScale; }
		virtual inline void SetScale(const glm::vec3& scale) { mScale = scale; }
		virtual inline void Scale(const glm::vec3& scale) { mScale += scale; }
		
	protected:
		glm::vec3 mTranslation;
		glm::vec3 mRotation;
		glm::vec3 mScale;
		
	};
	
}
