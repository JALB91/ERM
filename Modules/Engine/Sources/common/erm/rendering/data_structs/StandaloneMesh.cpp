#include "erm/rendering/data_structs/StandaloneMesh.h"

#include "erm/rendering/Device.h"

#include "erm/utils/VkUtils.h"

namespace erm {

StandaloneMesh::StandaloneMesh(Device& device, Mesh&& mesh)
	: Mesh(std::move(mesh))
	, mDevice(device)
	, mIndexBuffer(
		  mDevice,
		  mIndicesData.size() * sizeof(IndexData),
		  static_cast<uint32_t>(mIndicesData.size()))
	, mVertexBuffer(mDevice, mVerticesData.size() * sizeof(VertexData))
{
	mIndexBuffer.Update(mIndicesData.data());
	mVertexBuffer.Update(mVerticesData.data());
	mIndBuffer = {mIndexBuffer.GetBuffer(), {0, mIndexBuffer.GetBufferSize()}};
	mVertBuffer = {mVertexBuffer.GetBuffer(), {0, mVertexBuffer.GetBufferSize()}};
}

StandaloneMesh::StandaloneMesh(
	Device& device,
	std::vector<VertexData>&& vertices,
	std::vector<IndexData>&& indices,
	const RenderConfigs& renderConfigs /*= RenderConfigs::DEFAULT_RENDER_CONFIGS*/,
	const PipelineConfigs& pipelineConfigs /*= PipelineConfigs::DEFAULT_PIPELINE_CONFIGS*/,
	const char* name /*= ""*/)
	: StandaloneMesh(device, Mesh(std::move(vertices), std::move(indices), renderConfigs, pipelineConfigs, name))
{}

StandaloneMesh::~StandaloneMesh()
{
	ERM_VK_CHECK(mDevice->waitIdle());
}

StandaloneMesh::StandaloneMesh(StandaloneMesh&& other)
	: Mesh(std::move(other))
	, mDevice(other.mDevice)
	, mIndexBuffer(std::move(other.mIndexBuffer))
	, mVertexBuffer(std::move(other.mVertexBuffer))
{}

} // namespace erm
