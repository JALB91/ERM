#include "rendering/Model.h"
#include "rendering/Renderer.h"
#include "rendering/VertexData.h"

#include "utils/Utils.h"

#include <iostream>

namespace erm {
	
	Model::Model(
		const char* path,
		const char* name,
		std::deque<Mesh>&& meshes
		)
		: mPath(path)
		, mName(name)
		, mMeshes(std::move(meshes))
		, mLocalBounds()
	{
		for (const Mesh& mesh: mMeshes)
		{
			if (const VertexData* verticesData = mesh.GetVerticesData())
			{
				for (int i = 0; i < mesh.GetVerticesDataCount(); ++i)
				{
					const Vertex& vertex = verticesData[i].mVertex;
					mLocalBounds = mLocalBounds.Expand(vertex);
				}
			}
		}
	}
	
	Model::~Model()
	{}
	
	Model::Model(Model&& other)
		: mPath(std::move(other.mPath))
		, mName(std::move(other.mName))
		, mMeshes(std::move(other.mMeshes))
		, mLocalBounds(std::move(other.mLocalBounds))
	{}
	
}
