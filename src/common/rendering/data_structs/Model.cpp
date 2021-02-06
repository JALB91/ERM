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
		VertexData* vertices,
		uint32_t verticesCount,
		IndexData* indices,
		uint32_t indicesCount,
		const RenderConfigs& configs /*= RenderConfigs::MODELS_RENDER_CONFIGS*/,
		const char* name /*= ""*/)
	{
		AddMesh({mDevice, vertices, verticesCount, indices, indicesCount, configs, name});
	}

	void Model::UpdateLocalBound()
	{
		mLocalBounds.Empty();
		for (const Mesh& mesh : mMeshes)
		{
			if (const VertexData* verticesData = mesh.GetVerticesData())
			{
				for (unsigned int i = 0; i < mesh.GetVerticesDataCount(); ++i)
				{
					const PositionVertex& vertex = verticesData[i].mPositionVertex;
					mLocalBounds = mLocalBounds.Expand(vertex);
				}
			}
		}
		SetDirty(true);
	}

} // namespace erm
