#include "rendering/Mesh.h"
#include "rendering/VertexData.h"
#include "rendering/VertexBuffer.h"
#include "rendering/VertexBufferLayout.h"
#include "rendering/IndexBuffer.h"
#include "rendering/VertexArray.h"

namespace erm {
	
	Mesh::Mesh(
		DrawMode drawMode,
		VertexData* vertices,
		unsigned int verticesCount,
		IndexData* indices,
		unsigned int indicesCount,
		Material* material, /*= nullptr*/
		const std::string& name /*= ""*/
		)
		: mDrawMode(drawMode)
		, mVerticesData(vertices)
		, mVerticesDataCount(verticesCount)
		, mIndicesData(indices)
		, mIndicesDataCount(indicesCount)
		, mMaterial(material)
		, mName(name)
		, mVB(nullptr)
		, mIB(nullptr)
		, mVA(nullptr)
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
		: mDrawMode(other.mDrawMode)
		, mVerticesData(other.mVerticesData)
		, mVerticesDataCount(other.mVerticesDataCount)
		, mIndicesData(other.mIndicesData)
		, mIndicesDataCount(other.mIndicesDataCount)
		, mMaterial(std::move(other.mMaterial))
		, mName(std::move(other.mName))
		, mVB(std::move(other.mVB))
		, mIB(std::move(other.mIB))
		, mVA(std::move(other.mVA))
	{
		other.mVerticesData = nullptr;
		other.mVerticesDataCount = 0;
		
		other.mIndicesData = nullptr;
		other.mIndicesDataCount = 0;
		
		other.mMaterial = nullptr;
		other.mName.clear();
		
		other.mVB.reset();
		other.mIB.reset();
		other.mVA.reset();
	}
		
	void Mesh::Setup()
	{
		if (
			IsReady() ||
			!mVerticesData ||
			mVerticesDataCount <= 0 ||
			!mIndicesData ||
			mIndicesDataCount <= 0
		)
		{
			return;
		}
		
		mVB = std::make_unique<VertexBuffer>(
			&mVerticesData[0].mPositionVertex[0],
			sizeof(VertexType) * kPositionVectorsLenght * mVerticesDataCount +
			sizeof(VertexType) * kNormalVectorsLenght * mVerticesDataCount +
			sizeof(VertexType) * kUVVectorsLenght * mVerticesDataCount
		);
		
		mIB = std::make_unique<IndexBuffer>(mIndicesData, mIndicesDataCount);
		
		VertexBufferLayout vbl;
		vbl.Push<VertexType>(kPositionVectorsLenght);
		vbl.Push<VertexType>(kNormalVectorsLenght);
		vbl.Push<VertexType>(kUVVectorsLenght);
		
		mVA = std::make_unique<VertexArray>();
		mVA->AddBuffer(*mVB, vbl);
	}
	
}
