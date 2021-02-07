#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/buffers/VertexData.h"
#include "erm/rendering/renderer/Renderer.h"

#include "erm/utils/Utils.h"

#include <iostream>

namespace erm {

	Model::Model(Device& device, const char* path, const char* name)
		: IAsset(path, name)
		, mDevice(device)
		, mIsDirty(false)
	{}

	Model::~Model()
	{}

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
		AddMesh({mDevice, std::move(vertices), std::move(indices), configs, name});
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

} // namespace erm
