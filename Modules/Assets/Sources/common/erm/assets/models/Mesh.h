#pragma once

#include "erm/assets/models/VertexData.h"
#include "erm/assets/models/IndexData.h"

#include <erm/math/BoundingBox.h>

#include <vector>

namespace erm {

struct Mesh
{
	Mesh(
		std::vector<VertexData>&& verticesData,
		std::vector<IndexData>&& indicesData)
		: mVerticesData(std::move(verticesData))
		, mIndicesData(std::move(indicesData))
	{
		UpdateLocalBounds();
	}
	
	void UpdateLocalBounds()
	{
		for (const VertexData& data : mVerticesData)
		{
			const PositionVertex& vertex = data.mPositionVertex;
			mLocalBounds = mLocalBounds.Expand(vertex);
		}
	}
	
	std::vector<VertexData> mVerticesData;
	std::vector<IndexData> mIndicesData;
	BoundingBox3D mLocalBounds;
};

} // namespace erm
