#pragma once

#include "rendering/IndexData.h"

#include <glm/glm.hpp>

#include <memory>

namespace erm {
	
	struct VertexData;
	class VertexBuffer;
	class IndexBuffer;
	class VertexArray;
	
	class Mesh
	{
	public:
		friend class MeshUtils;
		friend class ModelUtils;
		
	public:
		Mesh();
		~Mesh();
		
		Mesh(const Mesh& other);
		Mesh(Mesh&& other);
		
		Mesh& operator=(const Mesh& other);
		Mesh& operator=(Mesh&& other);
		
		inline const IndexBuffer& GetIB() const { return *mIB; }
		inline const VertexArray& GetVA() const { return *mVA; }
		
		inline const VertexData* GetVerticesData() const { return mVerticesData; }
		inline int GetVerticesDataCount() const { return mVerticesDataCount; }
		
		inline const IndexData* GetIndicesData() const { return mIndicesData; }
		inline int GetIndicesCount() const { return mIndicesDataCount; }
		
	private:
		void Setup();
		
		VertexData* mVerticesData;
		unsigned int mVerticesDataCount;
		
		IndexData* mIndicesData;
		unsigned int mIndicesDataCount;
		
		std::shared_ptr<VertexBuffer> mVB;
		std::shared_ptr<IndexBuffer> mIB;
		std::shared_ptr<VertexArray> mVA;
		
	};
	
}
