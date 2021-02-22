#include "erm/ray_tracing/RTBlas.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/data_structs/Model.h"

namespace erm {

	uint32_t RTBlas::sCurrentId = 0;

	RTBlas::RTBlas(
		Device& device,
		const Model& model)
		: mDevice(device)
		, mModel(model)
		, mID(sCurrentId++)
	{}

	RTBlas::~RTBlas()
	{}

	void RTBlas::UpdateBlasData()
	{
		ASSERT(mBlasData.mGeometries.size() == mBlasData.mInfos.size());
		const std::vector<Mesh>& meshes = mModel.GetMeshes();

		for (size_t i = mBlasData.mGeometries.size(); i < meshes.size(); ++i)
		{
			const Mesh& mesh = meshes[i];

			ASSERT(mesh.IsReady());

			vk::DeviceAddress vertexAddress = mDevice->getBufferAddress({mesh.GetVertexBuffer().GetBuffer()});
			vk::DeviceAddress indexAddress = mDevice->getBufferAddress({mesh.GetIndexBuffer().GetBuffer()});

			uint32_t maxPrimitiveCount = static_cast<uint32_t>(mesh.GetIndicesData().size()) / 3;

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
			triangles.setMaxVertex(static_cast<uint32_t>(mesh.GetVerticesData().size()));

			// Identify the above data as containing opaque triangles.
			vk::AccelerationStructureGeometryKHR& asGeom = mBlasData.mGeometries.emplace_back();
			asGeom.setGeometryType(vk::GeometryTypeKHR::eTriangles);
			asGeom.setFlags(vk::GeometryFlagBitsKHR::eOpaque);
			asGeom.geometry.setTriangles(triangles);

			// The entire array will be used to build the BLAS.
			vk::AccelerationStructureBuildRangeInfoKHR& offset = mBlasData.mInfos.emplace_back();
			offset.setFirstVertex(0);
			offset.setPrimitiveCount(maxPrimitiveCount);
			offset.setPrimitiveOffset(0);
			offset.setTransformOffset(0);
		}
	}

	void RTBlas::GetBuildInfo(vk::AccelerationStructureBuildGeometryInfoKHR& buildInfo) const
	{
		buildInfo.setGeometryCount(static_cast<uint32_t>(mBlasData.mGeometries.size()));
		buildInfo.setPGeometries(mBlasData.mGeometries.data());
		buildInfo.setType(vk::AccelerationStructureTypeKHR::eBottomLevel);
		buildInfo.setSrcAccelerationStructure(nullptr);
		buildInfo.setFlags(vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);
		buildInfo.setMode(vk::BuildAccelerationStructureModeKHR::eBuild);
	}

} // namespace erm