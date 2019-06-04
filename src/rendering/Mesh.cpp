#include "rendering/Mesh.h"
#include "rendering/VertexData.h"
#include "rendering/VertexBuffer.h"
#include "rendering/VertexBufferLayout.h"
#include "rendering/IndexBuffer.h"
#include "rendering/VertexArray.h"

namespace erm {
	
	Mesh::Mesh(const Material& material /* = Material::DEFAULT */)
		: mVerticesData(nullptr)
		, mVerticesDataCount(0)
		, mIndicesData(nullptr)
		, mIndicesDataCount(0)
		, mMaterial(material)
		, mVB(nullptr)
		, mIB(nullptr)
		, mVA(nullptr)
	{}
	
	Mesh::~Mesh()
	{
		if (mVerticesData)
		{
			free(mVerticesData);
		}
		
		if (mIndicesData)
		{
			free(mIndicesData);
		}
	}
	
	Mesh::Mesh(Mesh&& other)
		: mVerticesData(other.mVerticesData)
		, mVerticesDataCount(other.mVerticesDataCount)
		, mIndicesData(other.mIndicesData)
		, mIndicesDataCount(other.mIndicesDataCount)
		, mMaterial(other.mMaterial)
		, mVB(std::move(other.mVB))
		, mIB(std::move(other.mIB))
		, mVA(std::move(other.mVA))
	{
		other.mVerticesData = nullptr;
		other.mVerticesDataCount = 0;
		
		other.mIndicesData = nullptr;
		other.mIndicesDataCount = 0;
		
		other.mVB.reset();
		other.mIB.reset();
		other.mVA.reset();
	}
		
	void Mesh::Setup()
	{
		mVB = std::make_unique<VertexBuffer>(
			&mVerticesData[0].mVertex[0],
			sizeof(VertexType) * kVectorsLenght * mVerticesDataCount +
			sizeof(VertexType) * kUVVectorsLenght * mVerticesDataCount +
			sizeof(VertexType) * kNormalVectorsLenght * mVerticesDataCount
		);
		
		mIB = std::make_unique<IndexBuffer>(mIndicesData, mIndicesDataCount);
		
		VertexBufferLayout vbl;
		vbl.Push<VertexType>(kVectorsLenght);
		vbl.Push<VertexType>(kUVVectorsLenght);
		vbl.Push<VertexType>(kNormalVectorsLenght);
		
		mVA = std::make_unique<VertexArray>();
		mVA->AddBuffer(*mVB, vbl);
	}
	
}
