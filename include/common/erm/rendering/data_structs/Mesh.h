#pragma once

#include "erm/rendering/buffers/IndexData.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

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
			VertexData* vertices,
			uint32_t verticesCount,
			IndexData* indices,
			uint32_t indicesCount,
			const RenderConfigs& configs,
			const char* name = "");
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
		inline void SetName(const char* name) { mName = name; }

		inline const RenderConfigs& GetRenderConfigs() const { return mRenderConfigs; }
		inline void SetRenderConfigs(const RenderConfigs& configs) { mRenderConfigs = configs; }

		inline bool IsReady() const { return mVerticesData && mIndicesData && mVerticesDataCount > 0 && mIndicesDataCount > 0 && mVertexBuffer && mIndexBuffer; }

	private:
		Device& mDevice;
		RenderConfigs mRenderConfigs;

		std::string mName;

		VertexData* mVerticesData;
		uint32_t mVerticesDataCount;

		IndexData* mIndicesData;
		uint32_t mIndicesDataCount;

		std::unique_ptr<VertexBuffer> mVertexBuffer;
		std::unique_ptr<IndexBuffer> mIndexBuffer;
	};

} // namespace erm
