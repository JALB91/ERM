#pragma once

#include <glm/glm.hpp>

namespace erm {
	
	template<typename T>
	class BoundingBox
	{
	public:
		BoundingBox(const T& min, const T& max)
			: mMin(min)
			, mMax(max)
		{}
		
		BoundingBox(const T& size)
			: BoundingBox(size, size)
		{}
		
		BoundingBox()
			: BoundingBox(T(0.0f))
		{}
		
		T GetSize() const
		{
			return mMax - mMin;
		}
		
		void Empty();
		
		template<typename S>
		inline bool IsIntersecting(const BoundingBox<S>& other) const
		{
			return
				IsInside(other.mMin) ||
				IsInside(other.mMax) ||
				other.IsInside(mMin) ||
				other.IsInside(mMax);
		}
		
		template<typename S>
		bool IsInside(const S& point) const;
		
		template<typename S>
		BoundingBox Expand(const BoundingBox<S>& other) const;
		
		template<typename S>
		BoundingBox Intersect(const BoundingBox<S>& other) const;
		
		BoundingBox Expand(const glm::mat4& mat) const;
		BoundingBox Expand(const glm::vec3& vec) const;
		
	public:
		T mMin;
		T mMax;
		
	};
	
	typedef BoundingBox<glm::vec3> BoundingBox3D;
	typedef BoundingBox<glm::vec2> BoundingBox2D;
	
}
