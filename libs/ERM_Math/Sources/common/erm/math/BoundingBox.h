#pragma once

#include "erm/math/Types.h"

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
	{
		Empty();
	}

	BoundingBox(BoundingBox&&) noexcept = default;
	BoundingBox(const BoundingBox&) noexcept = default;

	BoundingBox& operator=(BoundingBox&&) noexcept = default;
	BoundingBox& operator=(const BoundingBox&) noexcept = default;

	T GetSize() const
	{
		return mMax - mMin;
	}

	void Empty()
	{
		mMin = T(INFINITY);
		mMax = T(-INFINITY);
	}

	template<typename S>
	inline bool IsIntersecting(const BoundingBox<S>& other) const
	{
		return IsInside(other.mMin) ||
			IsInside(other.mMax) ||
			other.IsInside(mMin) ||
			other.IsInside(mMax);
	}

	template<typename S>
	bool IsInside(const S& point) const;

	template<typename S>
	BoundingBox Expand(const BoundingBox<S>& other) const;

	template<typename S>
	BoundingBox Intersection(const BoundingBox<S>& other) const;

	BoundingBox Expand(const mat4& mat) const;
	BoundingBox Expand(const vec3& vec) const;

public:
	T mMin;
	T mMax;
};

typedef BoundingBox<vec3> BoundingBox3D;
typedef BoundingBox<vec2> BoundingBox2D;

} // namespace erm
