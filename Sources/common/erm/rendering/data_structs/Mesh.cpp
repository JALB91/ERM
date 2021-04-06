#include "erm/rendering/data_structs/Mesh.h"

namespace erm {

Mesh::Mesh(
	std::vector<VertexData>&& vertices,
	std::vector<IndexData>&& indices,
	const PipelineConfigs& configs /*= PipelineConfigs::DEFAULT_PIPELINE_CONFIGS*/,
	const char* name /*= ""*/
	)
	: mVerticesData(vertices)
	, mIndicesData(indices)
	, mPipelineConfigs(configs)
	, mVertBuffer(nullptr, {})
	, mIndBuffer(nullptr, {})
	, mName(name)
{
	ASSERT(!mVerticesData.empty() && !mIndicesData.empty());
}

Mesh::~Mesh() = default;

Mesh::Mesh(Mesh&& other)
	: mVerticesData(std::move(other.mVerticesData))
	, mIndicesData(std::move(other.mIndicesData))
	, mPipelineConfigs(std::move(other.mPipelineConfigs))
	, mVertBuffer(std::move(other.mVertBuffer))
	, mIndBuffer(std::move(other.mIndBuffer))
	, mName(std::move(other.mName))
{}

Mesh& Mesh::operator=(Mesh&& other)
{
	mVerticesData = std::move(other.mVerticesData);
	mIndicesData = std::move(other.mIndicesData);
	mPipelineConfigs = std::move(other.mPipelineConfigs);
	mVertBuffer = std::move(other.mVertBuffer);
	mIndBuffer = std::move(other.mIndBuffer);
	mName = std::move(other.mName);

	return *this;
}

} // namespace erm
