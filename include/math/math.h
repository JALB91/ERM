#pragma once

#include "rendering/VertexData.h"

namespace erm {
	
namespace math {

	bool VerticesIntersection(
		const Vertex& a,
		const Vertex& b,
		const Vertex& d1,
		const Vertex& d2,
		Vertex& intersection
	);

}
	
}
