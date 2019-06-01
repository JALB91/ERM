#include "math/math.h"

#include "math/mat.h"

namespace erm {
	
namespace math {
	
	bool VerticesIntersection(
		const Vertex& a,
		const Vertex& b,
		const Vertex& d1,
		const Vertex& d2,
		Vertex& intersection
	)
	{
		mat<2, 2, VertexType> m1;
		m1[0][0] = a.x - b.x;
		m1[0][1] = -d1.x;
		m1[1][0] = a.y - b.y;
		m1[1][1] = -d1.y;
		
		mat<2, 2, VertexType> m2;
		m2[0][0] = d2.x;
		m2[0][1] = -d1.x;
		m2[1][0] = d2.y;
		m2[1][1] = -d1.y;
		
		VertexType det2 = glm::determinant(m2);
		
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
		
		const VertexType det1 = glm::determinant(m1);
		const VertexType t = det1/det2;
		
		intersection = b + t * d2;
		
		return true;
	}

}
	
}
