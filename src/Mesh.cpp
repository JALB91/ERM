#include "Mesh.h"
#include "VertexData.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Utils.h"

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
		
		mesh.mIndicesDataCount = 1;
		mesh.mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexType) * kIndexVectorsLenght * mesh.mIndicesDataCount));
		mesh.mIndicesData[0] = IndexData(0, 1, 2);
		
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
		
		mesh.mIndicesDataCount = 2;
		mesh.mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexType) * kIndexVectorsLenght * mesh.mIndicesDataCount));
		mesh.mIndicesData[0] = IndexData(0, 1, 2);
		mesh.mIndicesData[1] = IndexData(1, 2, 3);
		
		mesh.Setup();
		
		return mesh;
	}
	
	Mesh::Mesh(const std::string& verticesStr, const std::string& indicesStr)
		: mTranslation(0.0f)
		, mRotation(0.0f)
		, mVerticesData(nullptr)
		, mIndicesData(nullptr)
	{
		std::vector<std::string> vertDataStr = SplitString(verticesStr, ';');
		
		mVerticesDataCount = vertDataStr.size();
		
		if (mVerticesDataCount > 0)
		{
			mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mVerticesDataCount));
		}
		
		for (int i = 0; i < mVerticesDataCount; ++i)
		{
			VertexData& currentVertex = mVerticesData[i];
			int coordReaded = 0;
			
			for (const std::string& value: SplitString(vertDataStr[i], ','))
			{
				int coord = std::atoi(value.c_str());
				
				switch (coordReaded)
				{
					case 0:
					case 1:
					case 2:
						currentVertex.mVertex[coordReaded] = coord;
						break;
					case 3:
					case 4:
						currentVertex.mTextureVertex[coordReaded - 3] = coord;
						break;
				}
				
				coordReaded++;
			}
		}
		
		std::vector<std::string> indDataStr = SplitString(indicesStr, ';');
		
		mIndicesDataCount = indDataStr.size();
		
		if (mIndicesDataCount)
		{
			mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexData) * mIndicesDataCount));
		}
		
		for (int i = 0; i < mIndicesDataCount; ++i)
		{
			IndexData& currentIndex = mIndicesData[i];
			int indexReaded = 0;
			
			for (const std::string& value: SplitString(indDataStr[i], ','))
			{
				int index = std::atoi(value.c_str());
				currentIndex[indexReaded++] = index;
			}
		}
		
		Setup();
	}
	
	Mesh::Mesh()
		: mTranslation(0.0f)
		, mRotation(0.0f)
		, mVerticesData(nullptr)
		, mVerticesDataCount(0)
		, mIndicesData(nullptr)
		, mIndicesDataCount(0)
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
		
		if (mIndicesData)
		{
			delete[] mIndicesData;
		}
	}
	
	Mesh::Mesh(const Mesh& other)
	{
		mTranslation = other.mTranslation;
		mRotation = other.mRotation;
		
		if (mVerticesData)
		{
			delete[] mVerticesData;
			mVerticesData = nullptr;
		}
		
		if (mIndicesData)
		{
			delete[] mIndicesData;
			mIndicesData = nullptr;
		}
		
		mVerticesDataCount = other.mVerticesDataCount;
		
		if (mVerticesDataCount > 0)
		{
			mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mVerticesDataCount));
			std::memcpy(mVerticesData, other.mVerticesData, mVerticesDataCount);
		}
		
		mIndicesDataCount = other.mIndicesDataCount;
		
		if (mIndicesDataCount > 0)
		{
			mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexType) * kIndexVectorsLenght * mIndicesDataCount));
			std::memcpy(mIndicesData, other.mIndicesData, mIndicesDataCount);
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
		
		mIndicesData = other.mIndicesData;
		mIndicesDataCount = other.mIndicesDataCount;
		
		mVBL = std::move(other.mVBL);
		mVB = std::move(other.mVB);
		mIB = std::move(other.mIB);
		mVA = std::move(other.mVA);
		
		other.mTranslation = glm::vec3(0.0f);
		other.mRotation = glm::vec3(0.0f);
		
		other.mVerticesData = nullptr;
		other.mVerticesDataCount = 0;
		
		other.mIndicesData = nullptr;
		other.mIndicesDataCount = 0;
		
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
		
		if (mVerticesData)
		{
			delete[] mVerticesData;
			mVerticesData = nullptr;
		}
		
		if (mIndicesData)
		{
			delete[] mIndicesData;
			mIndicesData = nullptr;
		}
		
		mVerticesDataCount = other.mVerticesDataCount;
		
		if (mVerticesDataCount > 0)
		{
			mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mVerticesDataCount));
			std::memcpy(mVerticesData, other.mVerticesData, mVerticesDataCount);
		}
		
		mIndicesDataCount = other.mIndicesDataCount;
		
		if (mIndicesDataCount > 0)
		{
			mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexType) * kIndexVectorsLenght * mIndicesDataCount));
			std::memcpy(mIndicesData, other.mIndicesData, mIndicesDataCount);
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
		
		if (mVerticesData)
		{
			delete[] mVerticesData;
		}
		
		if (mIndicesData)
		{
			delete[] mIndicesData;
		}
		
		mVerticesData = other.mVerticesData;
		mVerticesDataCount = other.mVerticesDataCount;
		
		mIndicesData = other.mIndicesData;
		mIndicesDataCount = other.mIndicesDataCount;
		
		mVBL = std::move(other.mVBL);
		mVB = std::move(other.mVB);
		mIB = std::move(other.mIB);
		mVA = std::move(other.mVA);
		
		other.mTranslation = glm::vec3(0.0f);
		other.mRotation = glm::vec3(0.0f);
		
		other.mVerticesData = nullptr;
		other.mVerticesDataCount = 0;
		
		other.mIndicesData = nullptr;
		other.mIndicesDataCount = 0;
		
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
		
		mIB = std::make_shared<IndexBuffer>(&mIndicesData[0][0], kIndexVectorsLenght * mIndicesDataCount);
		
		mVA = std::make_shared<VertexArray>();
		mVA->AddBuffer(*mVB, *mVBL);
	}
	
}
