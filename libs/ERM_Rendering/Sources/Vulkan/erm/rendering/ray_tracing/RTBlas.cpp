#include "erm/rendering/ray_tracing/RTBlas.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/models/GPUModel.h"
#include "erm/rendering/models/GPUMesh.h"

#include <erm/math/Types.h>

#include <erm/assets/models/Mesh.h>

namespace erm {

RTBlas::RTBlas(
	Device& device,
	const GPUModel& model)
	: mDevice(device)
	, mModel(model)
	, mNeedsBuild(false)
{}

RTBlas::~RTBlas() = default;

void RTBlas::updateBlasData()
{
	const auto& meshes = mModel.getGPUMeshes();
	const auto& vLayout = mModel.getVerticesBuffer().getLayout();
	const auto& iLayout = mModel.getIndicesBuffer().getLayout();

	vk::DeviceAddress vertexAddress = mDevice->getBufferAddress({vLayout.mBuffer});
	vk::DeviceAddress indexAddress = mDevice->getBufferAddress({iLayout.mBuffer});

	u32 maxVertex = 0;
	u32 maxPrimCount = 0;

	for (u64 i = 0; i < meshes.size(); ++i)
	{
		const auto& gpuMesh = meshes[i];
		const auto& mesh = gpuMesh.getMesh();

		maxVertex += static_cast<u32>(mesh.mVerticesData.size());
		maxPrimCount += static_cast<u32>(mesh.mIndicesData.size() / 3);
	}

	mNeedsBuild = true;

	// Describe buffer as array of VertexObj.
	vk::AccelerationStructureGeometryTrianglesDataKHR triangles;
	triangles.setVertexFormat(vk::Format::eR32G32B32Sfloat); // vec3 vertex position data.
	triangles.setVertexData(vertexAddress);
	triangles.setVertexStride(sizeof(VertexData));
	// Describe index data (32-bit unsigned int)
	triangles.setIndexType(vk::IndexType::eUint32);
	triangles.setIndexData(indexAddress);
	// Indicate identity transform by setting transformData to null device pointer.
	triangles.setTransformData({});
	triangles.setMaxVertex(maxVertex);

	// Identify the above data as containing opaque triangles.
	mBlasData.mGeometries.setGeometryType(vk::GeometryTypeKHR::eTriangles);
	mBlasData.mGeometries.setFlags(vk::GeometryFlagBitsKHR::eOpaque);
	mBlasData.mGeometries.geometry.setTriangles(triangles);

	// The entire array will be used to build the BLAS.
	mBlasData.mInfos.setFirstVertex(0);
	mBlasData.mInfos.setPrimitiveOffset(0);
	mBlasData.mInfos.setTransformOffset(0);
	mBlasData.mInfos.setPrimitiveCount(maxPrimCount);
}

void RTBlas::getBuildInfo(vk::AccelerationStructureBuildGeometryInfoKHR& buildInfo) const
{
	buildInfo.setGeometryCount(1);
	buildInfo.setPGeometries(&mBlasData.mGeometries);
	buildInfo.setType(vk::AccelerationStructureTypeKHR::eBottomLevel);
	buildInfo.setSrcAccelerationStructure(nullptr);
	buildInfo.setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);
	buildInfo.setMode(vk::BuildAccelerationStructureModeKHR::eBuild);
}

} // namespace erm
