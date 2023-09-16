#include "erm/rendering/models/GPUModel.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/assets/models/Mesh.h>

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

void GPUModel::UpdateBuffers()
{
	// UPDATE BUFFERS
	BufferLayout vLayout;
	BufferLayout iLayout;

	vLayout.mInfos.reserve(mGPUMeshes.size());
	iLayout.mInfos.reserve(mGPUMeshes.size());

	size_t vTargetOffset = 0;
	size_t iTargetOffset = 0;
	uint32_t totalIndices = 0;

	for (const GPUMesh& gpuMesh : mGPUMeshes)
	{
		const Mesh& mesh = gpuMesh.GetMesh();
		const BufferInfo& vInfo = vLayout.mInfos.emplace_back(vTargetOffset, mesh.mVerticesData.size() * sizeof(VertexData));
		const BufferInfo& iInfo = iLayout.mInfos.emplace_back(iTargetOffset, mesh.mIndicesData.size() * sizeof(IndexData));

		vTargetOffset += vInfo.mStride;
		iTargetOffset += iInfo.mStride;
		totalIndices += static_cast<uint32_t>(mesh.mIndicesData.size());
	}

	if (!mVerticesBuffer || mVerticesBuffer->GetBufferSize() < vTargetOffset)
		mVerticesBuffer = std::make_unique<VertexBuffer>(mDevice, vTargetOffset);
	if (!mIndicesBuffer || mIndicesBuffer->GetBufferSize() < iTargetOffset)
		mIndicesBuffer = std::make_unique<IndexBuffer>(mDevice, iTargetOffset, totalIndices);

	if (mIndicesBuffer->GetIndicesCount() != totalIndices)
		mIndicesBuffer->SetIndicesCount(totalIndices);

	vLayout.mBuffer = mVerticesBuffer->GetBuffer();
	iLayout.mBuffer = mIndicesBuffer->GetBuffer();

	for (size_t i = 0; i < mGPUMeshes.size(); ++i)
	{
		GPUMesh& gpuMesh = mGPUMeshes[i];
		const Mesh& mesh = gpuMesh.GetMesh();
		const BufferInfo& vInfo = vLayout.mInfos[i];
		const BufferInfo& iInfo = iLayout.mInfos[i];

		gpuMesh.SetVertBufferHandle(BufferHandle(mVerticesBuffer->GetBuffer(), vInfo));
		gpuMesh.SetIndBufferHandle(BufferHandle(mIndicesBuffer->GetBuffer(), iInfo));

		mVerticesBuffer->Update(mesh.mVerticesData.data(), vInfo);
		mIndicesBuffer->Update(mesh.mIndicesData.data(), iInfo);
	}

	mVerticesBuffer->SetLayout(std::move(vLayout));
	mIndicesBuffer->SetLayout(std::move(iLayout));

#ifdef ERM_RAY_TRACING_ENABLED
	// UPDATE BLAS DATA
	mBlas.UpdateBlasData();
#endif
}

} // namespace erm
