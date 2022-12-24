#pragma once

#include "erm/resources/models/VertexData.h"
#include "erm/resources/models/IndexTypes.h"

#include <string>
#include <string_view>
#include <vector>

namespace erm {

class Mesh
{
public:
	Mesh(
		std::vector<VertexData>&& vertexData,
		std::vector<IndexData>&& indexData,
		std::string_view name = "")
		: mVertexData(std::move(vertexData))
		, mIndexData(std::move(indexData))
		, mName(name)
	{}
	
	inline const std::vector<VertexData>& GetVerticesData() const
	{
		return mVertexData;
	}
	
	inline const std::vector<IndexData>& GetIndicesData() const
	{
		return mIndexData;
	}

	inline const std::string& GetName() const { return mName; }
	inline void SetName(std::string_view name) { mName = name; }

private:
	std::vector<VertexData> mVertexData;
	std::vector<IndexData> mIndexData;
	std::string mName;
	
};

} // namespace erm
