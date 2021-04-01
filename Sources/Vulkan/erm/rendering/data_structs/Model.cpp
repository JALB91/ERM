#include "erm/rendering/data_structs/Model.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/data_structs/VertexData.h"

#include "erm/utils/Utils.h"

#include <iostream>

namespace erm {

Model::Model(Device& device, const char* path, const char* name)
	: IAsset(path, name)
	, mDevice(device)
#ifdef ERM_RAY_TRACING_ENABLED
	, mBlas(device, *this)
#endif
	, mIsDirty(false)
{}

Model::~Model()
{
	mDevice->waitIdle();
}

void Model::AddMesh(Mesh&& mesh)
{
	mMeshes.emplace_back(std::move(mesh));
	UpdateLocalBound();
}

void Model::AddMesh(
	std::vector<VertexData>&& vertices,
	std::vector<IndexData>&& indices,
	const RenderConfigs& configs /*= RenderConfigs::MODELS_RENDER_CONFIGS*/,
	const char* name /*= ""*/)
{
	AddMesh({std::move(vertices), std::move(indices), configs, name});
}

void Model::UpdateLocalBound()
{
	mLocalBounds.Empty();
	for (const Mesh& mesh : mMeshes)
	{
		for (const VertexData& data : mesh.GetVerticesData())
		{
			const PositionVertex& vertex = data.mPositionVertex;
			mLocalBounds = mLocalBounds.Expand(vertex);
		}
	}
	SetDirty(true);
}

void Model::UpdateBuffers()
{
	// UPDATE BUFFERS
	BufferLayout vLayout;
	BufferLayout iLayout;

	vLayout.mInfos.reserve(mMeshes.size());
	iLayout.mInfos.reserve(mMeshes.size());

	size_t vTargetOffset = 0;
	size_t iTargetOffset = 0;
	uint32_t totalIndices = 0;

	for (const Mesh& mesh : mMeshes)
	{
		const BufferInfo& vInfo = vLayout.mInfos.emplace_back(vTargetOffset, mesh.GetVerticesData().size() * sizeof(VertexData));
		const BufferInfo& iInfo = iLayout.mInfos.emplace_back(iTargetOffset, mesh.GetIndicesData().size() * sizeof(IndexData));

		vTargetOffset += vInfo.mStride;
		iTargetOffset += iInfo.mStride;
		totalIndices += static_cast<uint32_t>(mesh.GetIndicesData().size());
	}

	mVerticesBuffer = std::make_unique<VertexBuffer>(mDevice, vTargetOffset);
	mIndicesBuffer = std::make_unique<IndexBuffer>(mDevice, iTargetOffset, totalIndices);

	vLayout.mBuffer = mVerticesBuffer->GetBuffer();
	iLayout.mBuffer = mIndicesBuffer->GetBuffer();

	for (size_t i = 0; i < mMeshes.size(); ++i)
	{
		Mesh& mesh = mMeshes[i];
		const BufferInfo& vInfo = vLayout.mInfos[i];
		const BufferInfo& iInfo = iLayout.mInfos[i];

		mesh.mVertBuffer = BufferHandle(mVerticesBuffer->GetBuffer(), vInfo);
		mesh.mIndBuffer = BufferHandle(mIndicesBuffer->GetBuffer(), iInfo);

		mVerticesBuffer->Update(mesh.GetVerticesData().data(), vInfo);
		mIndicesBuffer->Update(mesh.GetIndicesData().data(), iInfo);
	}

	mVerticesBuffer->SetLayout(std::move(vLayout));
	mIndicesBuffer->SetLayout(std::move(iLayout));

#ifdef ERM_RAY_TRACING_ENABLED
	// UPDATE BLAS DATA
	mBlas.UpdateBlasData();
#endif
}

} // namespace erm
