#pragma once

#include "erm/rendering/buffers/IndexData.h"
#include "erm/rendering/enums/DrawMode.h"

#include <memory>
#include <string>

namespace erm {
	class ShaderProgram;
	class Texture;
	class VertexBuffer;
	class IndexBuffer;
	class Device;
	struct VertexData;
	struct Material;
} // namespace erm

namespace erm {

	class Mesh
	{
	public:
		Mesh(
			Device& device,
			DrawMode drawMode,
			VertexData* vertices,
			uint32_t verticesCount,
			IndexData* indices,
			uint32_t indicesCount,
			const char* name = "",
			ShaderProgram* shaderProgram = nullptr,
			Material* material = nullptr,
			Texture* texture = nullptr);
		~Mesh();

		Mesh(Mesh&& other);
		Mesh(const Mesh&) = delete;

		Mesh& operator=(Mesh&&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		void Setup();

		inline const VertexBuffer& GetVertexBuffer() const { return *mVertexBuffer; }
		inline const IndexBuffer& GetIndexBuffer() const { return *mIndexBuffer; }

		inline const VertexData* GetVerticesData() const { return mVerticesData; }
		inline uint32_t GetVerticesDataCount() const { return mVerticesDataCount; }

		inline const IndexData* GetIndicesData() const { return mIndicesData; }
		inline uint32_t GetIndicesCount() const { return mIndicesDataCount; }

		inline const std::string& GetName() const { return mName; }
		inline void SetName(const std::string& name) { mName = name; }

		inline DrawMode GetDrawMode() const { return mDrawMode; }
		inline void SetDrawMode(DrawMode drawMode) { mDrawMode = drawMode; }

		inline ShaderProgram* GetShaderProgram() const { return mShaderProgram; }
		inline void SetShaderProgram(ShaderProgram* shaderProgram) { mShaderProgram = shaderProgram; }

		inline Material* GetMaterial() const { return mMaterial; }
		inline void SetMaterial(Material* material) { mMaterial = material; }

		inline Texture* GetTexture() const { return mTexture; }
		inline void SetTexture(Texture* texture) { mTexture = texture; }

		inline bool IsReady() const { return mVerticesData && mIndicesData && mVerticesDataCount > 0 && mIndicesDataCount > 0 && mVertexBuffer && mIndexBuffer; }

	private:
		Device& mDevice;
		DrawMode mDrawMode;

		std::string mName;

		VertexData* mVerticesData;
		uint32_t mVerticesDataCount;

		IndexData* mIndicesData;
		uint32_t mIndicesDataCount;

		ShaderProgram* mShaderProgram;
		Material* mMaterial;
		Texture* mTexture;

		std::unique_ptr<VertexBuffer> mVertexBuffer;
		std::unique_ptr<IndexBuffer> mIndexBuffer;
	};

} // namespace erm
