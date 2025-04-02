#include "erm/rendering/models/GPUModel.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/assets/models/Mesh.h>

#include <erm/math/Types.h>

#include <erm/utils/Utils.h>

#include <iostream>

namespace erm {

GPUModel::GPUModel(Device& device, const Model& model)
	: mDevice(device)
	, mModel(model)
#ifdef ERM_RAY_TRACING_ENABLED
	, mBlas(device, *this)
#endif
{}

GPUModel::~GPUModel()
{
	ERM_VK_CHECK(mDevice->waitIdle());
}

void GPUModel::updateBuffers()
{
	// UPDATE BUFFERS
	BufferLayout vLayout;
	BufferLayout iLayout;

	vLayout.mInfos.reserve(mGPUMeshes.size());
	iLayout.mInfos.reserve(mGPUMeshes.size());

	u64 vTargetOffset = 0;
	u64 iTargetOffset = 0;
	u32 totalIndices = 0;

	for (const auto& gpuMesh : mGPUMeshes)
	{
		const auto& mesh = gpuMesh.getMesh();
		const auto& vInfo = vLayout.mInfos.emplace_back(vTargetOffset, mesh.mVerticesData.size() * sizeof(VertexData));
		const auto& iInfo = iLayout.mInfos.emplace_back(iTargetOffset, mesh.mIndicesData.size() * sizeof(IndexData));

		vTargetOffset += vInfo.mStride;
		iTargetOffset += iInfo.mStride;
		totalIndices += static_cast<u32>(mesh.mIndicesData.size());
	}

	if (!mVerticesBuffer || mVerticesBuffer->getBufferSize() < vTargetOffset)
	{
		mVerticesBuffer = std::make_unique<VertexBuffer>(mDevice, vTargetOffset);
	}
	if (!mIndicesBuffer || mIndicesBuffer->getBufferSize() < iTargetOffset)
	{
		mIndicesBuffer = std::make_unique<IndexBuffer>(mDevice, iTargetOffset, totalIndices);
	}

	if (mIndicesBuffer->getIndicesCount() != totalIndices)
	{
		mIndicesBuffer->setIndicesCount(totalIndices);
	}

	vLayout.mBuffer = mVerticesBuffer->getBuffer();
	iLayout.mBuffer = mIndicesBuffer->getBuffer();

	for (u64 i = 0; i < mGPUMeshes.size(); ++i)
	{
		auto& gpuMesh = mGPUMeshes[i];
		const Mesh& mesh = gpuMesh.getMesh();
		const auto& vInfo = vLayout.mInfos[i];
		const auto& iInfo = iLayout.mInfos[i];

		gpuMesh.setVertBufferHandle(BufferHandle(mVerticesBuffer->getBuffer(), vInfo));
		gpuMesh.setIndBufferHandle(BufferHandle(mIndicesBuffer->getBuffer(), iInfo));

		mVerticesBuffer->update(mesh.mVerticesData.data(), vInfo);
		mIndicesBuffer->update(mesh.mIndicesData.data(), iInfo);
	}

	mVerticesBuffer->setLayout(std::move(vLayout));
	mIndicesBuffer->setLayout(std::move(iLayout));

#ifdef ERM_RAY_TRACING_ENABLED
	// UPDATE BLAS DATA
	mBlas.updateBlasData();
#endif
}

} // namespace erm
