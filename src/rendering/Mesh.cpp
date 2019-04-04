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
		: ITransformable()
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
			free(mVerticesData);
		}
		
		if (mIndicesData)
		{
			free(mIndicesData);
		}
	}
	
	Mesh::Mesh(const Mesh& other)
	{
		mTranslation = other.mTranslation;
		mRotation = other.mRotation;
		mScale = other.mScale;
		
		mVerticesDataCount = other.mVerticesDataCount;
		
		if (mVerticesDataCount > 0)
		{
			mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mVerticesDataCount));
			std::memcpy(mVerticesData, other.mVerticesData, mVerticesDataCount);
		}
		
		mIndicesDataCount = other.mIndicesDataCount;
		
		if (mIndicesDataCount > 0)
		{
			mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexType) * mIndicesDataCount));
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
		mScale = other.mScale;
		
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
		mScale = other.mScale;
		
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
			std::memcpy(mVerticesData, other.mVerticesData, mVerticesDataCount);
		}
		
		mIndicesDataCount = other.mIndicesDataCount;
		
		if (mIndicesDataCount > 0)
		{
			mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexType) * mIndicesDataCount));
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
		mScale = other.mScale;
		
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
		
		mIB = std::make_shared<IndexBuffer>(mIndicesData, mIndicesDataCount);
		
		mVA = std::make_shared<VertexArray>();
		mVA->AddBuffer(*mVB, *mVBL);
	}
	
}
