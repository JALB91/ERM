#pragma once

#include <glm/glm.hpp>

#include <memory>

namespace erm {
	
	struct VertexData;
	class VertexBuffer;
	class VertexBufferLayout;
	class IndexBuffer;
	class VertexArray;
	
	class Mesh
	{
	public:
		static Mesh CreateTriangle();
		static Mesh CreateSquare();
		
	public:
		Mesh();
		~Mesh();
		
		Mesh(const Mesh& other);
		Mesh(Mesh&& other);
		
		Mesh& operator=(const Mesh& other);
		Mesh& operator=(Mesh&& other);
		
		inline glm::vec3& GetTranslation() { return mTranslation; }
		inline const glm::vec3& GetTranslation() const { return mTranslation; }
		inline void SetTranslation(const glm::vec3& translation) { mTranslation = translation; }
		
		inline glm::vec3& GetRotation() { return mRotation; }
		inline const glm::vec3& GetRotation() const { return mRotation; }
		inline void SetRotation(const glm::vec3& rotation) { mRotation = rotation; }
		
		inline const IndexBuffer& GetIB() const { return *mIB; }
		inline const VertexArray& GetVA() const { return *mVA; }
		
	private:
		static constexpr unsigned int kIndexVectorsLenght = 3;
		
		typedef unsigned int IndexType;
		typedef glm::vec<kIndexVectorsLenght, IndexType> Index;
		
	private:
		void Setup();
		
		glm::vec3 mTranslation;
		glm::vec3 mRotation;
		
		VertexData* mVerticesData;
		unsigned int mVerticesDataCount;
		
		Index* mIndices;
		unsigned int mIndicesCount;
		
		std::shared_ptr<VertexBufferLayout> mVBL;
		std::shared_ptr<VertexBuffer> mVB;
		std::shared_ptr<IndexBuffer> mIB;
		std::shared_ptr<VertexArray> mVA;
		
	};
	
}
