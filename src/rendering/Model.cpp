#include "rendering/Model.h"
#include "rendering/Renderer.h"
#include "rendering/VertexData.h"

#include "utils/Utils.h"

#include <iostream>

namespace erm {
	
	Model::Model(const char* path, const char* name)
		: mPath(path)
		, mName(name)
	{}
	
	Model::~Model()
	{}

	void Model::AddMesh(Mesh&& mesh)
	{
		mMeshes.emplace_back(std::move(mesh));
		UpdateLocalBound();
	}

	void Model::UpdateLocalBound()
	{
		mLocalBounds.Empty();
		for (const Mesh& mesh : mMeshes)
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
	
}
