#include "erm/resources/models/Model.h"

#include "erm/resources/models/Mesh.h"

namespace erm {

Model::Model(std::string_view path, std::string_view name)
	: IAsset(path, name)
{}

void Model::AddMesh(const Mesh& mesh)
{
	mMeshes.emplace_back(&mesh);
	
	for (const VertexData& data : mesh.GetVerticesData())
	{
		const PositionVertex& vertex = data.mPositionVertex;
		mLocalBounds = mLocalBounds.Expand(vertex);
	}
}

void Model::UpdateLocalBounds()
{
	mLocalBounds.Empty();
	
	for (const auto* mesh : mMeshes)
	{
		for (const VertexData& data : mesh->GetVerticesData())
		{
			const PositionVertex& vertex = data.mPositionVertex;
			mLocalBounds = mLocalBounds.Expand(vertex);
		}
	}
}

} // namespace erm
