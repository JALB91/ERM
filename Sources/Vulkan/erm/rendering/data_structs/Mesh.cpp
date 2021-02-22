#include "erm/rendering/data_structs/Mesh.h"

#include "erm/rendering/Device.h"

#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"

namespace erm {

	Mesh::Mesh(
		Device& device,
		std::vector<VertexData>&& vertices,
		std::vector<IndexData>&& indices,
		const RenderConfigs& configs /*= RenderConfigs::MODELS_RENDER_CONFIGS*/,
		const char* name /*= ""*/
		)
		: mDevice(device)
		, mRenderConfigs(configs)
		, mName(name)
		, mVerticesData(vertices)
		, mIndicesData(indices)
	{
		ASSERT(!mVerticesData.empty() && !mIndicesData.empty());
	}

	Mesh::~Mesh()
	{
		mDevice->waitIdle();
	}

	Mesh::Mesh(Mesh&& other)
		: mDevice(other.mDevice)
		, mRenderConfigs(std::move(other.mRenderConfigs))
		, mName(std::move(other.mName))
		, mVerticesData(std::move(other.mVerticesData))
		, mIndicesData(std::move(other.mIndicesData))
		, mVertexBuffer(std::move(other.mVertexBuffer))
		, mIndexBuffer(std::move(other.mIndexBuffer))
	{}

	void Mesh::Setup()
	{
		if (
			IsReady() ||
			mVerticesData.empty() ||
			mIndicesData.empty())
		{
			return;
		}

		mVertexBuffer = std::make_unique<VertexBuffer>(
			mDevice,
			mVerticesData.size() * sizeof(VertexData),
			mVerticesData.data());

		mIndexBuffer = std::make_unique<IndexBuffer>(
			mDevice,
			mIndicesData.size() * sizeof(IndexData),
			mIndicesData.data(),
			static_cast<uint32_t>(mIndicesData.size()));
	}

} // namespace erm
