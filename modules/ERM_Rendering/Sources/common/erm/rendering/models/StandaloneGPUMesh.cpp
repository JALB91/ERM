#include "erm/rendering/models/StandaloneGPUMesh.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/assets/models/Mesh.h>

namespace erm {

StandaloneGPUMesh::StandaloneGPUMesh(
	Device& device,
	const Mesh& mesh
	)
	: GPUMesh(mesh)
	, mDevice(device)
	, mIndexBuffer(
		  mDevice,
		  mesh.mIndicesData.size() * sizeof(IndexData),
		  static_cast<u32>(mesh.mIndicesData.size()))
	, mVertexBuffer(mDevice, mesh.mVerticesData.size() * sizeof(VertexData))
{
	mIndexBuffer.update(mesh.mIndicesData.data());
	mVertexBuffer.update(mesh.mVerticesData.data());
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
