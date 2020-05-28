#include "erm/rendering/data_structs/Mesh.h"

#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/buffers/VertexData.h"

namespace erm {

	Mesh::Mesh(
		Device& device,
		DrawMode drawMode,
		VertexData* vertices,
		uint32_t verticesCount,
		IndexData* indices,
		uint32_t indicesCount,
		const char* name /*= ""*/,
		ShaderProgram* shaderProgram /*= nullptr*/,
		Material* material /*= nullptr*/,
		Texture* texture /*= nullptr*/
		)
		: mDevice(device)
		, mDrawMode(drawMode)
		, mName(name)
		, mVerticesData(vertices)
		, mVerticesDataCount(verticesCount)
		, mIndicesData(indices)
		, mIndicesDataCount(indicesCount)
		, mShaderProgram(shaderProgram)
		, mMaterial(material)
		, mTexture(texture)
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
		, mDrawMode(other.mDrawMode)
		, mName(std::move(other.mName))
		, mVerticesData(other.mVerticesData)
		, mVerticesDataCount(other.mVerticesDataCount)
		, mIndicesData(other.mIndicesData)
		, mIndicesDataCount(other.mIndicesDataCount)
		, mShaderProgram(other.mShaderProgram)
		, mMaterial(other.mMaterial)
		, mTexture(other.mTexture)
		, mVertexBuffer(std::move(other.mVertexBuffer))
		, mIndexBuffer(std::move(other.mIndexBuffer))
	{
		other.mVerticesData = nullptr;
		other.mVerticesDataCount = 0;

		other.mIndicesData = nullptr;
		other.mIndicesDataCount = 0;

		other.mShaderProgram = nullptr;
		other.mMaterial = nullptr;
		other.mTexture = nullptr;

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

		mVertexBuffer = std::make_unique<VertexBuffer>(
			mDevice,
			&mVerticesData[0].mPositionVertex[0],
			sizeof(VertexData) * mVerticesDataCount);

		mIndexBuffer = std::make_unique<IndexBuffer>(
			mDevice,
			mIndicesData,
			mIndicesDataCount * sizeof(IndexData),
			mIndicesDataCount);
	}

} // namespace erm
