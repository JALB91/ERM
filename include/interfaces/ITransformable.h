#pragma once

#include "math/vec.h"

namespace erm {
	
	class ITransformable
	{
	public:
		inline ITransformable(
			const math::vec3& translation,
			const math::vec3& rotation,
			const math::vec3& scale
		)
			: mTranslation(translation)
			, mRotation(rotation)
			, mScale(scale)
		{}
		inline ITransformable()
		: ITransformable(
			math::vec3(0.0f),
			math::vec3(0.0f),
			math::vec3(1.0f)
		)
		{}
		virtual ~ITransformable() = default;
		
		inline math::vec3 GetTranslation() { return mTranslation; }
		inline const math::vec3& GetTranslation() const { return mTranslation; }
		virtual inline void SetTranslation(const math::vec3& translation) { mTranslation = translation; }
		virtual inline void Translate(const math::vec3& translation) { mTranslation += translation; }
		
		inline math::vec3 GetRotation() { return mRotation; }
		inline const math::vec3& GetRotation() const { return mRotation; }
		virtual inline void SetRotation(const math::vec3& rotation) { mRotation = rotation; }
		virtual inline void Rotate(const math::vec3& rotation) { mRotation += rotation; }
		
		inline math::vec3 GetScale() { return mScale; }
		inline const math::vec3& GetScale() const { return mScale; }
		virtual inline void SetScale(const math::vec3& scale) { mScale = scale; }
		virtual inline void Scale(const math::vec3& scale) { mScale += scale; }
		
	protected:
		math::vec3 mTranslation;
		math::vec3 mRotation;
		math::vec3 mScale;
		
	};
	
}
