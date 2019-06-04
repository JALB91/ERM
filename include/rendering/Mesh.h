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
		Mesh(const Material& material = Material::DEFAULT);
		~Mesh();
		
		Mesh(Mesh&& other);
		Mesh(const Mesh&) = delete;
		
		Mesh& operator=(Mesh&&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		
		inline void SetMaterial(const Material& material) { mMaterial = material; }
		inline const Material& GetMaterial() const { return mMaterial.get(); }
		
		inline const IndexBuffer& GetIB() const { return *mIB; }
		inline const VertexArray& GetVA() const { return *mVA; }
		
		inline const VertexData* const GetVerticesData() const { return mVerticesData; }
		inline int GetVerticesDataCount() const { return mVerticesDataCount; }
		
		inline const IndexData* const GetIndicesData() const { return mIndicesData; }
		inline int GetIndicesCount() const { return mIndicesDataCount; }
		
	private:
		void Setup();
		
		VertexData* mVerticesData;
		unsigned int mVerticesDataCount;
		
		IndexData* mIndicesData;
		unsigned int mIndicesDataCount;
		
		std::reference_wrapper<const Material> mMaterial;
		
		std::unique_ptr<VertexBuffer> mVB;
		std::unique_ptr<IndexBuffer> mIB;
		std::unique_ptr<VertexArray> mVA;
		
	};
	
}
