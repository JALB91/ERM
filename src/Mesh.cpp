#include "Mesh.h"
#include "VertexData.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

#include <cstring>

namespace erm {
	
	Mesh Mesh::CreateTriangle()
	{
		Mesh mesh;
		
		mesh.mVerticesDataCount = 3;
		mesh.mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mesh.mVerticesDataCount));
		mesh.mVerticesData[0].mVertex = Vertex(-1.0f, -1.0f, 0.0f);
		mesh.mVerticesData[1].mVertex = Vertex(-1.0f,  1.0f, 0.0f);
		mesh.mVerticesData[2].mVertex = Vertex( 1.0f, -1.0f, 0.0f);
		mesh.mVerticesData[0].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[1].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[2].mTextureVertex = TextureVertex(1.0f, 0.0f);
		
		mesh.mIndicesCount = 1;
		mesh.mIndices = static_cast<Index*>(malloc(sizeof(IndexType) * kIndexVectorsLenght * mesh.mIndicesCount));
		mesh.mIndices[0] = Index(0, 1, 2);
		
		mesh.Setup();
		
		return mesh;
	}
	
	Mesh Mesh::CreateSquare()
	{
		Mesh mesh;
		
		mesh.mVerticesDataCount = 4;
		mesh.mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mesh.mVerticesDataCount));
		mesh.mVerticesData[0].mVertex = Vertex(-1.0f, -1.0f, 0.0f);
		mesh.mVerticesData[1].mVertex = Vertex(-1.0f,  1.0f, 0.0f);
		mesh.mVerticesData[2].mVertex = Vertex( 1.0f, -1.0f, 0.0f);
		mesh.mVerticesData[3].mVertex = Vertex( 1.0f,  1.0f, 0.0f);
		mesh.mVerticesData[0].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[1].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[2].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[3].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		mesh.mIndicesCount = 2;
		mesh.mIndices = static_cast<Index*>(malloc(sizeof(IndexType) * kIndexVectorsLenght * mesh.mIndicesCount));
		mesh.mIndices[0] = Index(0, 1, 2);
		mesh.mIndices[1] = Index(1, 2, 3);
		
		mesh.Setup();
		
		return mesh;
	}
	
	Mesh::Mesh()
		: mTranslation(0.0f)
		, mRotation(0.0f)
		, mVerticesData(nullptr)
		, mVerticesDataCount(0)
		, mIndices(nullptr)
		, mIndicesCount(0)
		, mVBL(nullptr)
		, mVB(nullptr)
		, mIB(nullptr)
		, mVA(nullptr)
	{}
	
	Mesh::~Mesh()
	{
		if (mVerticesData)
		{
			delete[] mVerticesData;
		}
		
		if (mIndices)
		{
			delete[] mIndices;
		}
	}
	
	Mesh::Mesh(const Mesh& other)
	{
		mTranslation = other.mTranslation;
		mRotation = other.mRotation;
		
		if (mVerticesDataCount > 0)
		{
			delete[] mVerticesData;
			mVerticesData = nullptr;
		}
		
		if (mIndicesCount > 0)
		{
			delete[] mIndices;
			mIndices = nullptr;
		}
		
		mVerticesDataCount = other.mVerticesDataCount;
		
		if (mVerticesDataCount > 0)
		{
			mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mVerticesDataCount));
			std::memcpy(mVerticesData, other.mVerticesData, mVerticesDataCount);
		}
		
		mIndicesCount = other.mIndicesCount;
		
		if (mIndicesCount > 0)
		{
			mIndices = static_cast<Index*>(malloc(sizeof(IndexType) * kIndexVectorsLenght * mIndicesCount));
			std::memcpy(mIndices, other.mIndices, mIndicesCount);
		}
		
		mVBL = other.mVBL;
		mVB = other.mVB;
		mIB = other.mIB;
		mVA = other.mVA;
	}
	
	Mesh::Mesh(Mesh&& other)
	{
		mTranslation = other.mTranslation;
		mRotation = other.mRotation;
		
		mVerticesData = other.mVerticesData;
		mVerticesDataCount = other.mVerticesDataCount;
		
		mIndices = other.mIndices;
		mIndicesCount = other.mIndicesCount;
		
		mVBL = std::move(other.mVBL);
		mVB = std::move(other.mVB);
		mIB = std::move(other.mIB);
		mVA = std::move(other.mVA);
		
		other.mTranslation = glm::vec3(0.0f);
		other.mRotation = glm::vec3(0.0f);
		
		other.mVerticesData = nullptr;
		other.mVerticesDataCount = 0;
		
		other.mIndices = nullptr;
		other.mIndicesCount = 0;
		
		other.mVBL = nullptr;
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
		
		mTranslation = other.mTranslation;
		mRotation = other.mRotation;
		
		delete[] mVerticesData;
		delete[] mIndices;
		
		mVerticesDataCount = other.mVerticesDataCount;
		
		if (mVerticesDataCount > 0)
		{
			mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mVerticesDataCount));
			std::memcpy(mVerticesData, other.mVerticesData, mVerticesDataCount);
		}
		
		mIndicesCount = other.mIndicesCount;
		
		if (mIndicesCount > 0)
		{
			mIndices = static_cast<Index*>(malloc(sizeof(IndexType) * kIndexVectorsLenght * mIndicesCount));
			std::memcpy(mIndices, other.mIndices, mIndicesCount);
		}
		
		mVBL = other.mVBL;
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
		
		mTranslation = other.mTranslation;
		mRotation = other.mRotation;
		
		delete[] mVerticesData;
		delete[] mIndices;
		
		mVerticesData = other.mVerticesData;
		mVerticesDataCount = other.mVerticesDataCount;
		
		mIndices = other.mIndices;
		mIndicesCount = other.mIndicesCount;
		
		mVBL = std::move(other.mVBL);
		mVB = std::move(other.mVB);
		mIB = std::move(other.mIB);
		mVA = std::move(other.mVA);
		
		other.mTranslation = glm::vec3(0.0f);
		other.mRotation = glm::vec3(0.0f);
		
		other.mVerticesData = nullptr;
		other.mVerticesDataCount = 0;
		
		other.mIndices = nullptr;
		other.mIndicesCount = 0;
		
		other.mVBL = nullptr;
		other.mVB = nullptr;
		other.mIB = nullptr;
		other.mVA = nullptr;
		
		return *this;
	}
	
	void Mesh::Setup()
	{
		mVB = std::make_shared<VertexBuffer>(&mVerticesData[0].mVertex[0], sizeof(VertexType) * kVectorsLenght * mVerticesDataCount + sizeof(TextureVertexType) * kTextureVectorsLenght * mVerticesDataCount);
		
		mVBL = std::make_shared<VertexBufferLayout>();
		mVBL->Push<VertexType>(kVectorsLenght);
		mVBL->Push<TextureVertexType>(kTextureVectorsLenght);
		
		mIB = std::make_shared<IndexBuffer>(&mIndices[0][0], kIndexVectorsLenght * mIndicesCount);
		
		mVA = std::make_shared<VertexArray>();
		mVA->AddBuffer(*mVB, *mVBL);
	}
	
}
