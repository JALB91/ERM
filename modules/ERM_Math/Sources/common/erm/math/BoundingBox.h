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
		empty();
	}

	BoundingBox(BoundingBox&&) noexcept = default;
	BoundingBox(const BoundingBox&) noexcept = default;

	BoundingBox& operator=(BoundingBox&&) noexcept = default;
	BoundingBox& operator=(const BoundingBox&) noexcept = default;

	T getSize() const
	{
		return mMax - mMin;
	}

	void empty()
	{
		mMin = T(INFINITY);
		mMax = T(-INFINITY);
	}

	template<typename S>
	inline bool isIntersecting(const BoundingBox<S>& other) const
	{
		return isInside(other.mMin) ||
			isInside(other.mMax) ||
			other.isInside(mMin) ||
			other.isInside(mMax);
	}

	template<typename S>
	bool isInside(const S& point) const;

	template<typename S>
	BoundingBox expand(const BoundingBox<S>& other) const;

	template<typename S>
	BoundingBox intersection(const BoundingBox<S>& other) const;

	BoundingBox expand(const mat4& mat) const;
	BoundingBox expand(const vec3& vec) const;

public:
	T mMin;
	T mMax;
};

typedef BoundingBox<vec3> BoundingBox3D;
typedef BoundingBox<vec2> BoundingBox2D;

} // namespace erm
