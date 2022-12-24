#include "erm/rendering/models/StandaloneGPUMesh.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/resources/models/Mesh.h>

namespace erm {

StandaloneGPUMesh::StandaloneGPUMesh(
	Device& device,
	const Mesh& mesh,
	const RenderConfigs& renderConfigs /* = RenderConfigs::DEFAULT*/,
	const PipelineConfigs& pipelineConfigs /* = PipelineConfigs::DEFAULT*/
	)
	: GPUMesh(mesh, renderConfigs, pipelineConfigs)
	, mDevice(device)
	, mIndexBuffer(
		  mDevice,
		  mesh.GetIndicesData().size() * sizeof(IndexData),
		  static_cast<uint32_t>(mesh.GetIndicesData().size()))
	, mVertexBuffer(mDevice, mesh.GetVerticesData().size() * sizeof(VertexData))
{
	mIndexBuffer.Update(mesh.GetIndicesData().data());
	mVertexBuffer.Update(mesh.GetVerticesData().data());
}

StandaloneGPUMesh::~StandaloneGPUMesh()
{
	ERM_VK_CHECK(mDevice->waitIdle());
}

StandaloneGPUMesh::StandaloneGPUMesh(StandaloneGPUMesh&& other) noexcept
	: GPUMesh(std::move(other))
	, mDevice(other.mDevice)
	, mIndexBuffer(std::move(other.mIndexBuffer))
	, mVertexBuffer(std::move(other.mVertexBuffer))
{}

} // namespace erm
