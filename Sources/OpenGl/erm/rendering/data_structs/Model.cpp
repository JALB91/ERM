#include "erm/rendering/data_structs/Mesh.h"

#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/buffers/VertexBufferLayout.h"
#include "erm/rendering/buffers/VertexData.h"

namespace erm {

	Mesh::Mesh(
		Device& device,
		VertexData* vertices,
		uint32_t verticesCount,
		IndexData* indices,
		uint32_t indicesCount,
		const RenderConfigs& configs /*= {}*/,
		const char* name /*= ""*/
		)
		: mDevice(device)
		, mRenderConfigs(configs)
		, mName(name)
		, mVerticesData(vertices)
		, mVerticesDataCount(verticesCount)
		, mIndicesData(indices)
		, mIndicesDataCount(indicesCount)
	{}

	Mesh::~Mesh()
	{
		if (mVerticesData && mVerticesDataCount > 0)
		{
			delete[] mVerticesData;
		}

		if (mIndicesData && mIndicesDataCount > 0)
		{
			delete[] mIndicesData;
		}
	}

	Mesh::Mesh(Mesh&& other)
		: mDevice(other.mDevice)
		, mRenderConfigs(std::move(other.mRenderConfigs))
		, mName(std::move(other.mName))
		, mVerticesData(other.mVerticesData)
		, mVerticesDataCount(other.mVerticesDataCount)
		, mIndicesData(other.mIndicesData)
		, mIndicesDataCount(other.mIndicesDataCount)
		, mVertexBuffer(std::move(other.mVertexBuffer))
		, mIndexBuffer(std::move(other.mIndexBuffer))
	{
		other.mVerticesData = nullptr;
		other.mVerticesDataCount = 0;

		other.mIndicesData = nullptr;
		other.mIndicesDataCount = 0;

		other.mVertexBuffer.reset();
		other.mIndexBuffer.reset();
	}

	void Mesh::Setup()
	{
		if (
			IsReady() ||
			!mVerticesData ||
			mVerticesDataCount <= 0 ||
			!mIndicesData ||
			mIndicesDataCount <= 0)
		{
			return;
		}

		VertexBufferLayout vbl;
		vbl.Push<VertexType>(kPositionVectorsLenght);
		vbl.Push<VertexType>(kNormalVectorsLenght);
		vbl.Push<VertexType>(kUVVectorsLenght);
		vbl.Push<VertexType>(kMaxBonesNumber);
		vbl.Push<IdType>(kMaxBonesNumber);

		mVertexBuffer = std::make_unique<VertexBuffer>(
			std::move(vbl),
			&mVerticesData[0].mPositionVertex[0],
			sizeof(VertexData) * mVerticesDataCount);

		mIndexBuffer = std::make_unique<IndexBuffer>(
			mIndicesData,
			mIndicesDataCount * sizeof(IndexData),
			mIndicesDataCount);
	}

} // namespace erm
