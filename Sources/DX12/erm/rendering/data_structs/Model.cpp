#include "erm/rendering/data_structs/Model.h"

#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"

namespace erm {

	Model::Model(Device& device, const char* path, const char* name)
		: IAsset(path, name)
		, mDevice(device)
	{}

	Model::~Model()
	{}

	void Model::AddMesh(Mesh&& mesh)
	{}

	void Model::AddMesh(
		std::vector<VertexData>&& vertices,
		std::vector<IndexData>&& indices,
		const RenderConfigs& configs /*= RenderConfigs::MODELS_RENDER_CONFIGS*/,
		const char* name /*= ""*/)
	{}

	void Model::UpdateLocalBound()
	{}

	void Model::UpdateBuffers()
	{}

} // namespace erm