#include "erm/rendering/data_structs/Mesh.h"

namespace erm {

	Mesh::Mesh(
		std::vector<VertexData>&& vertices,
		std::vector<IndexData>&& indices,
		const RenderConfigs& configs /*= RenderConfigs::MODELS_RENDER_CONFIGS*/,
		const char* name /*= ""*/
		)
		: mVerticesData(vertices)
		, mIndicesData(indices)
		, mRenderConfigs(configs)
		, mName(name)
	{
		ASSERT(!mVerticesData.empty() && !mIndicesData.empty());
	}

	Mesh::~Mesh() = default;

	Mesh::Mesh(Mesh&& other)
		: mVerticesData(std::move(other.mVerticesData))
		, mIndicesData(std::move(other.mIndicesData))
		, mRenderConfigs(std::move(other.mRenderConfigs))
		, mName(std::move(other.mName))
	{}

	Mesh& Mesh::operator=(Mesh&& other)
	{
		mVerticesData = std::move(other.mVerticesData);
		mIndicesData = std::move(other.mIndicesData);
		mRenderConfigs = std::move(other.mRenderConfigs);
		mName = std::move(other.mName);

		return *this;
	}

} // namespace erm
