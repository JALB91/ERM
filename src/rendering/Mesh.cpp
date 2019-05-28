#include "rendering/Mesh.h"
#include "rendering/VertexData.h"
#include "rendering/VertexBuffer.h"
#include "rendering/VertexBufferLayout.h"
#include "rendering/IndexBuffer.h"
#include "rendering/VertexArray.h"
#include "utils/Utils.h"

#include <cstring>

namespace erm {
	
	Mesh::Mesh()
		: mVerticesData(nullptr)
		, mVerticesDataCount(0)
		, mIndicesData(nullptr)
		, mIndicesDataCount(0)
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
	
	Mesh::Mesh(const Mesh& other)
	{
		mVerticesDataCount = other.mVerticesDataCount;
		
		if (mVerticesDataCount > 0)
		{
			mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mVerticesDataCount));
			std::memcpy(mVerticesData, other.mVerticesData, sizeof(VertexData) * mVerticesDataCount);
		}
		
		mIndicesDataCount = other.mIndicesDataCount;
		
		if (mIndicesDataCount > 0)
		{
			mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexData) * mIndicesDataCount));
			std::memcpy(mIndicesData, other.mIndicesData, sizeof(IndexData) * mIndicesDataCount);
		}
		
		mVB = other.mVB;
		mIB = other.mIB;
		mVA = other.mVA;
	}
	
	Mesh::Mesh(Mesh&& other)
	{
		mVerticesData = other.mVerticesData;
		mVerticesDataCount = other.mVerticesDataCount;
		
		mIndicesData = other.mIndicesData;
		mIndicesDataCount = other.mIndicesDataCount;
		
		mVB = std::move(other.mVB);
		mIB = std::move(other.mIB);
		mVA = std::move(other.mVA);
		
		other.mVerticesData = nullptr;
		other.mVerticesDataCount = 0;
		
		other.mIndicesData = nullptr;
		other.mIndicesDataCount = 0;
		
		other.mVB = nullptr;
		other.mIB = nullptr;
		other.mVA = nullptr;
	}
	
	Mesh& Mesh::operator=(const Mesh& other)
	{
		if (&other == this)
		{
			return *this;
		}
		
		if (mVerticesData)
		{
			free(mVerticesData);
			mVerticesData = nullptr;
		}
		
		if (mIndicesData)
		{
			free(mIndicesData);
			mIndicesData = nullptr;
		}
		
		mVerticesDataCount = other.mVerticesDataCount;
		
		if (mVerticesDataCount > 0)
		{
			mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mVerticesDataCount));
			std::memcpy(mVerticesData, other.mVerticesData, sizeof(VertexData) * mVerticesDataCount);
		}
		
		mIndicesDataCount = other.mIndicesDataCount;
		
		if (mIndicesDataCount > 0)
		{
			mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexData) * mIndicesDataCount));
			std::memcpy(mIndicesData, other.mIndicesData, sizeof(IndexData) * mIndicesDataCount);
		}
		
		mVB = other.mVB;
		mIB = other.mIB;
		mVA = other.mVA;
		
		return *this;
	}
	
	Mesh& Mesh::operator=(Mesh&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		
		if (mVerticesData)
		{
			free(mVerticesData);
		}
		
		if (mIndicesData)
		{
			free(mIndicesData);
		}
		
		mVerticesData = other.mVerticesData;
		mVerticesDataCount = other.mVerticesDataCount;
		
		mIndicesData = other.mIndicesData;
		mIndicesDataCount = other.mIndicesDataCount;
		
		mVB = std::move(other.mVB);
		mIB = std::move(other.mIB);
		mVA = std::move(other.mVA);
		
		other.mVerticesData = nullptr;
		other.mVerticesDataCount = 0;
		
		other.mIndicesData = nullptr;
		other.mIndicesDataCount = 0;
		
		other.mVB = nullptr;
		other.mIB = nullptr;
		other.mVA = nullptr;
		
		return *this;
	}
	
	void Mesh::Setup()
	{
		mVB = std::make_shared<VertexBuffer>(
			&mVerticesData[0].mVertex[0],
			sizeof(VertexType) * kVectorsLenght * mVerticesDataCount +
			sizeof(VertexType) * kNormalVectorsLenght * mVerticesDataCount +
			sizeof(VertexType) * kTextureVectorsLenght * mVerticesDataCount
		);
		
		VertexBufferLayout vbl;
		vbl.Push<VertexType>(kVectorsLenght);
		vbl.Push<VertexType>(kNormalVectorsLenght);
		vbl.Push<VertexType>(kTextureVectorsLenght);
		
		mIB = std::make_shared<IndexBuffer>(mIndicesData, mIndicesDataCount);
		
		mVA = std::make_shared<VertexArray>();
		mVA->AddBuffer(*mVB, vbl);
	}
	
}
