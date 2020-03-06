#pragma once

#include "erm/rendering/buffers/IndexData.h"
#include "erm/rendering/data_structs/Material.h"
#include "erm/rendering/enums/DrawMode.h"

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
		Mesh(
			 DrawMode drawMode,
			 VertexData* vertices,
			 unsigned int verticesCount,
			 IndexData* indices,
			 unsigned int indicesCount,
			 Material* material = nullptr,
			 const std::string& name = ""
		);
		~Mesh();
		
		Mesh(Mesh&& other);
		Mesh(const Mesh&) = delete;
		
		Mesh& operator=(Mesh&&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		
		void Setup();
		
		inline Material* GetMaterial() const { return mMaterial; }
		inline void SetMaterial(Material* material) { mMaterial = material; }
		
		inline const IndexBuffer& GetIB() const { return *mIB; }
		inline const VertexArray& GetVA() const { return *mVA; }
		
		inline const VertexData* GetVerticesData() const { return mVerticesData; }
		inline unsigned int GetVerticesDataCount() const { return mVerticesDataCount; }
		
		inline const IndexData* GetIndicesData() const { return mIndicesData; }
		inline unsigned int GetIndicesCount() const { return mIndicesDataCount; }
		
		inline DrawMode GetDrawMode() const { return mDrawMode; }
		
		inline const std::string& GetName() const { return mName; }
		inline void SetName(const std::string& name) { mName = name; }

		inline bool IsReady() const { return mVerticesData && mIndicesData && mVerticesDataCount > 0 && mIndicesDataCount > 0 && mIB && mVA && mVB; }
		
	private:
		const DrawMode mDrawMode;
		
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
