#include "math/math.h"

#include "math/mat.h"

namespace erm {
	namespace math {
		
		void DecomposeMatrix(
			const math::mat4& matrix,
			math::vec3& translation,
			math::quat& rotation,
			math::vec3& scale
		)
		{
			translation.x = matrix[3][0];
			translation.y = matrix[3][1];
			translation.z = matrix[3][2];
			
			rotation = glm::quat_cast(matrix);
			
			scale.x = glm::length(matrix[0]);
			scale.y = glm::length(matrix[1]);
			scale.z = glm::length(matrix[2]);
		}
		
		bool VerticesIntersection(
			const math::vec3& a,
			const math::vec3& b,
			const math::vec3& d1,
			const math::vec3& d2,
			math::vec3& intersection
		)
		{
			mat2 m1;
			m1[0][0] = a.x - b.x;
			m1[0][1] = -d1.x;
			m1[1][0] = a.y - b.y;
			m1[1][1] = -d1.y;
			
			mat2 m2;
			m2[0][0] = d2.x;
			m2[0][1] = -d1.x;
			m2[1][0] = d2.y;
			m2[1][1] = -d1.y;
			
			float det2 = glm::determinant(m2);
			
			if (det2 == 0.0f)
			{
				m2[0][0] = d2.y;
				m2[0][1] = -d1.y;
				m2[1][0] = d2.z;
				m2[1][1] = -d1.z;
				
				det2 = glm::determinant(m2);
				
				if (det2 == 0.0f)
				{
					m2[0][0] = d2.x;
					m2[0][1] = -d1.x;
					m2[1][0] = d2.z;
					m2[1][1] = -d1.z;
					
					det2 = glm::determinant(m2);
					
					if (det2 == 0.0f)
					{
						return false;
					}
					
					m1[0][0] = a.x - b.x;
					m1[0][1] = -d1.x;
					m1[1][0] = a.z - b.z;
					m1[1][1] = -d1.z;
				}
				else
				{
					m1[0][0] = a.y - b.y;
					m1[0][1] = -d1.y;
					m1[1][0] = a.z - b.z;
					m1[1][1] = -d1.z;
				}
			}
			
			const float det1 = glm::determinant(m1);
			const float t = det1/det2;
			
			intersection = b + t * d2;
			
			return true;
		}

	}
}
