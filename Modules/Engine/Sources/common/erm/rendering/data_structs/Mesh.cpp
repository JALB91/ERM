#include "erm/rendering/data_structs/Mesh.h"

namespace erm {

Mesh::Mesh(
	std::vector<VertexData>&& vertices,
	std::vector<IndexData>&& indices,
	const RenderConfigs& renderConfigs /*= RenderConfigs::DEFAULT_RENDER_CONFIGS*/,
	const PipelineConfigs& pipelineConfigs /*= PipelineConfigs::DEFAULT_PIPELINE_CONFIGS*/,
	const char* name /*= ""*/
	)
	: mVerticesData(vertices)
	, mIndicesData(indices)
	, mRenderConfigs(renderConfigs)
	, mPipelineConfigs(pipelineConfigs)
	, mVertBuffer(nullptr, {})
	, mIndBuffer(nullptr, {})
	, mName(name)
{
	ERM_ASSERT(!mVerticesData.empty() && !mIndicesData.empty());
}

Mesh::~Mesh() = default;

Mesh::Mesh(Mesh&& other) noexcept
	: mVerticesData(std::move(other.mVerticesData))
	, mIndicesData(std::move(other.mIndicesData))
	, mRenderConfigs(std::move(other.mRenderConfigs))
	, mPipelineConfigs(std::move(other.mPipelineConfigs))
	, mVertBuffer(std::move(other.mVertBuffer))
	, mIndBuffer(std::move(other.mIndBuffer))
	, mName(std::move(other.mName))
{}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	mVerticesData = std::move(other.mVerticesData);
	mIndicesData = std::move(other.mIndicesData);
	mRenderConfigs = std::move(other.mRenderConfigs);
	mPipelineConfigs = std::move(other.mPipelineConfigs);
	mVertBuffer = std::move(other.mVertBuffer);
	mIndBuffer = std::move(other.mIndBuffer);
	mName = std::move(other.mName);

	return *this;
}

} // namespace erm
