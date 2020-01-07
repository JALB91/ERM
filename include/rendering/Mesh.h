#pragma once

#include "rendering/IndexData.h"
#include "rendering/Material.h"

#include <glm/glm.hpp>

#include <memory>
#include <functional>

namespace erm {
	
	struct VertexData;
	struct Material;
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
		
		Mesh(Mesh&& other);
		Mesh(const Mesh&) = delete;
		
		Mesh& operator=(Mesh&&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		
		inline Material* GetMaterial() const { return mMaterial; }
		inline void SetMaterial(Material* material) { mMaterial = material; }
		
		inline const IndexBuffer& GetIB() const { return *mIB; }
		inline const VertexArray& GetVA() const { return *mVA; }
		
		inline const VertexData* GetVerticesData() const { return mVerticesData; }
		inline unsigned int GetVerticesDataCount() const { return mVerticesDataCount; }
		
		inline const IndexData* GetIndicesData() const { return mIndicesData; }
		inline unsigned int GetIndicesCount() const { return mIndicesDataCount; }
		
		inline const std::string& GetName() const { return mName; }
		inline void SetName(const std::string& name) { mName = name; }

		inline bool IsReady() const { return mVerticesData && mIndicesData && mVerticesDataCount > 0 && mIndicesDataCount > 0 && mIB && mVA && mVB; }
		
	private:
		void Setup();
		
		VertexData* mVerticesData;
		unsigned int mVerticesDataCount;
		
		IndexData* mIndicesData;
		unsigned int mIndicesDataCount;
		
		Material* mMaterial;
		
		std::string mName;
		
		std::unique_ptr<VertexBuffer> mVB;
		std::unique_ptr<IndexBuffer> mIB;
		std::unique_ptr<VertexArray> mVA;
		
	};
	
}
