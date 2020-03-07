#include "erm/math/BoundingBox.h"

namespace erm {
	
	template<>
	template<>
	bool BoundingBox3D::IsInside(const math::vec3& point) const
	{
		return
			mMin.x <= point.x &&
			mMin.y <= point.y &&
			mMin.z <= point.z &&
			mMax.x >= point.x &&
			mMax.y >= point.y &&
			mMax.z >= point.z;
	}
	
	template<>
	template<>
	bool BoundingBox3D::IsInside(const math::vec2& point) const
	{
		return
			mMin.x <= point.x &&
			mMin.y <= point.y &&
			mMax.x >= point.x &&
			mMax.y >= point.y;
	}
	
	template<>
	template<typename S>
	bool BoundingBox2D::IsInside(const S& point) const
	{
		return
			mMin.x <= point.x &&
			mMin.y <= point.y &&
			mMax.x >= point.x &&
			mMax.y >= point.y;
	}
	
	template<>
	template<>
	BoundingBox3D BoundingBox3D::Expand(const BoundingBox3D& other) const
	{
		BoundingBox3D result;
		
		result.mMin.x = std::fmin(mMin.x, other.mMin.x);
		result.mMin.y = std::fmin(mMin.y, other.mMin.y);
		result.mMin.z = std::fmin(mMin.z, other.mMin.z);
		
		result.mMax.x = std::fmax(mMax.x, other.mMax.x);
		result.mMax.y = std::fmax(mMax.y, other.mMax.y);
		result.mMax.z = std::fmax(mMax.z, other.mMax.z);
		
		return result;
	}
	
	template<>
	template<>
	BoundingBox3D BoundingBox3D::Expand(const BoundingBox2D& other) const
	{
		BoundingBox3D result;
		
		result.mMin.x = std::fmin(mMin.x, other.mMin.x);
		result.mMin.y = std::fmin(mMin.y, other.mMin.y);
		result.mMin.z = mMin.z;
		
		result.mMax.x = std::fmax(mMax.x, other.mMax.x);
		result.mMax.y = std::fmax(mMax.y, other.mMax.y);
		result.mMax.z = mMax.z;
		
		return result;
	}
	
	template<>
	template<typename S>
	BoundingBox2D BoundingBox2D::Expand(const BoundingBox<S>& other) const
	{
		BoundingBox2D result;
		
		result.mMin.x = std::fmin(mMin.x, other.mMin.x);
		result.mMin.y = std::fmin(mMin.y, other.mMin.y);
		
		result.mMax.x = std::fmax(mMax.x, other.mMax.x);
		result.mMax.y = std::fmax(mMax.y, other.mMax.y);
		
		return result;
	}
	
	template<>
	template<>
	BoundingBox3D BoundingBox3D::Intersect(const BoundingBox3D& other) const
	{
		if (!IsIntersecting(other)) return BoundingBox3D();
		
		BoundingBox3D result;
		
		result.mMin.x = std::fmax(mMin.x, other.mMin.x);
		result.mMin.y = std::fmax(mMin.y, other.mMin.y);
		result.mMin.z = std::fmax(mMin.z, other.mMin.z);
		
		result.mMax.x = std::fmin(mMax.x, other.mMax.x);
		result.mMax.y = std::fmin(mMax.y, other.mMax.y);
		result.mMax.z = std::fmin(mMax.z, other.mMax.z);
		
		return result;
	}
	
	template<>
	template<>
	BoundingBox3D BoundingBox3D::Intersect(const BoundingBox2D& other) const
	{
		if (!IsIntersecting(other)) return BoundingBox3D();
		
		BoundingBox3D result;
		
		result.mMin.x = std::fmax(mMin.x, other.mMin.x);
		result.mMin.y = std::fmax(mMin.y, other.mMin.y);
		result.mMin.z = mMin.z;
		
		result.mMax.x = std::fmin(mMax.x, other.mMax.x);
		result.mMax.y = std::fmin(mMax.y, other.mMax.y);
		result.mMax.z = mMax.z;
		
		return result;
	}
	
	template<>
	template<typename S>
	BoundingBox2D BoundingBox2D::Intersect(const BoundingBox<S>& other) const
	{
		if (!IsIntersecting(other)) return BoundingBox2D();
		
		BoundingBox2D result;
		
		result.mMin.x = std::fmax(mMin.x, other.mMin.x);
		result.mMin.y = std::fmax(mMin.y, other.mMin.y);
		
		result.mMax.x = std::fmin(mMax.x, other.mMax.x);
		result.mMax.y = std::fmin(mMax.y, other.mMax.y);
		
		return result;
	}
	
	template<>
	BoundingBox2D BoundingBox2D::Expand(const math::mat4& mat) const
	{
		BoundingBox2D result;
		
		result.mMin.x = result.mMax.x = mat[3][0];
		result.mMin.y = result.mMax.y = mat[3][1];
		
		if (mat[0][0] > 0.0f)
		{
			result.mMin.x += mat[0][0] * mMin.x;
			result.mMax.x += mat[0][0] * mMax.x;
		}
		else
		{
			result.mMin.x += mat[0][0] * mMax.x;
			result.mMax.x += mat[0][0] * mMin.x;
		}
		
		if (mat[0][1] > 0.0f)
		{
			result.mMin.y += mat[0][1] * mMin.y;
			result.mMax.y += mat[0][1] * mMax.y;
		}
		else
		{
			result.mMin.y += mat[0][1] * mMax.y;
			result.mMax.y += mat[0][1] * mMin.y;
		}
		
		if (mat[1][0] > 0.0f)
		{
			result.mMin.x += mat[1][0] * mMin.x;
			result.mMax.x += mat[1][0] * mMax.x;
		}
		else
		{
			result.mMin.x += mat[1][0] * mMax.x;
			result.mMax.x += mat[1][0] * mMin.x;
		}
		
		if (mat[1][1] > 0.0f)
		{
			result.mMin.y += mat[1][1] * mMin.y;
			result.mMax.y += mat[1][1] * mMax.y;
		}
		else
		{
			result.mMin.y += mat[1][1] * mMax.y;
			result.mMax.y += mat[1][1] * mMin.y;
		}
		
		if (mat[2][0] > 0.0f)
		{
			result.mMin.x += mat[2][0] * mMin.x;
			result.mMax.x += mat[2][0] * mMax.x;
		}
		else
		{
			result.mMin.x += mat[2][0] * mMax.x;
			result.mMax.x += mat[2][0] * mMin.x;
		}
		
		if (mat[2][1] > 0.0f)
		{
			result.mMin.y += mat[2][1] * mMin.y;
			result.mMax.y += mat[2][1] * mMax.y;
		}
		else
		{
			result.mMin.y += mat[2][1] * mMax.y;
			result.mMax.y += mat[2][1] * mMin.y;
		}
		
		return result;
	}
	
	template<>
	BoundingBox3D BoundingBox3D::Expand(const math::mat4& mat) const
	{
		BoundingBox3D result;
		
		result.mMin.x = result.mMax.x = mat[3][0];
		result.mMin.y = result.mMax.y = mat[3][1];
		result.mMin.z = result.mMax.z = mat[3][2];
		
		if (mat[0][0] > 0.0f)
		{
			result.mMin.x += mat[0][0] * mMin.x;
			result.mMax.x += mat[0][0] * mMax.x;
		}
		else
		{
			result.mMin.x += mat[0][0] * mMax.x;
			result.mMax.x += mat[0][0] * mMin.x;
		}
		
		if (mat[0][1] > 0.0f)
		{
			result.mMin.y += mat[0][1] * mMin.y;
			result.mMax.y += mat[0][1] * mMax.y;
		}
		else
		{
			result.mMin.y += mat[0][1] * mMax.y;
			result.mMax.y += mat[0][1] * mMin.y;
		}
		
		if (mat[0][2] > 0.0f)
		{
			result.mMin.z += mat[0][2] * mMin.z;
			result.mMax.z += mat[0][2] * mMax.z;
		}
		else
		{
			result.mMin.z += mat[0][2] * mMax.z;
			result.mMax.z += mat[0][2] * mMin.z;
		}
		
		if (mat[1][0] > 0.0f)
		{
			result.mMin.x += mat[1][0] * mMin.x;
			result.mMax.x += mat[1][0] * mMax.x;
		}
		else
		{
			result.mMin.x += mat[1][0] * mMax.x;
			result.mMax.x += mat[1][0] * mMin.x;
		}
		
		if (mat[1][1] > 0.0f)
		{
			result.mMin.y += mat[1][1] * mMin.y;
			result.mMax.y += mat[1][1] * mMax.y;
		}
		else
		{
			result.mMin.y += mat[1][1] * mMax.y;
			result.mMax.y += mat[1][1] * mMin.y;
		}
		
		if (mat[1][2] > 0.0f)
		{
			result.mMin.z += mat[1][2] * mMin.z;
			result.mMax.z += mat[1][2] * mMax.z;
		}
		else
		{
			result.mMin.z += mat[1][2] * mMax.z;
			result.mMax.z += mat[1][2] * mMin.z;
		}
		
		if (mat[2][0] > 0.0f)
		{
			result.mMin.x += mat[2][0] * mMin.x;
			result.mMax.x += mat[2][0] * mMax.x;
		}
		else
		{
			result.mMin.x += mat[2][0] * mMax.x;
			result.mMax.x += mat[2][0] * mMin.x;
		}
		
		if (mat[2][1] > 0.0f)
		{
			result.mMin.y += mat[2][1] * mMin.y;
			result.mMax.y += mat[2][1] * mMax.y;
		}
		else
		{
			result.mMin.y += mat[2][1] * mMax.y;
			result.mMax.y += mat[2][1] * mMin.y;
		}
		
		if (mat[2][2] > 0.0f)
		{
			result.mMin.z += mat[2][2] * mMin.z;
			result.mMax.z += mat[2][2] * mMax.z;
		}
		else
		{
			result.mMin.z += mat[2][2] * mMax.z;
			result.mMax.z += mat[2][2] * mMin.z;
		}
		
		return result;
	}
	
	template<>
	BoundingBox2D BoundingBox2D::Expand(const math::vec3& vec) const
	{
		BoundingBox2D result;
		
		result.mMin.x = std::fmin(vec.x, mMin.x);
		result.mMin.y = std::fmin(vec.y, mMin.y);
		
		result.mMax.x = std::fmax(vec.x, mMax.x);
		result.mMax.y = std::fmax(vec.y, mMax.y);
		
		return result;
	}
	
	template<>
	BoundingBox3D BoundingBox3D::Expand(const math::vec3& vec) const
	{
		BoundingBox3D result;
		
		result.mMin.x = std::fmin(vec.x, mMin.x);
		result.mMin.y = std::fmin(vec.y, mMin.y);
		result.mMin.z = std::fmin(vec.z, mMin.z);
		
		result.mMax.x = std::fmax(vec.x, mMax.x);
		result.mMax.y = std::fmax(vec.y, mMax.y);
		result.mMax.z = std::fmax(vec.z, mMax.z);
		
		return result;
	}
	
}
