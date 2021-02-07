#pragma once

#include "erm/rendering/buffers/IndexData.h"
#include "erm/rendering/buffers/VertexData.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <memory>
#include <string>
#include <vector>

namespace erm {
	class ShaderProgram;
	class Texture;
	class VertexBuffer;
	class IndexBuffer;
	class Device;
	struct Material;
} // namespace erm

namespace erm {

	class Mesh
	{
	public:
		Mesh(
			Device& device,
			std::vector<VertexData>&& vertices,
			std::vector<IndexData>&& indices,
			const RenderConfigs& configs = RenderConfigs::MODELS_RENDER_CONFIGS,
			const char* name = "");
		~Mesh();

		Mesh(Mesh&& other);
		Mesh(const Mesh&) = delete;

		Mesh& operator=(Mesh&&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		void Setup();

		inline const VertexBuffer& GetVertexBuffer() const { return *mVertexBuffer; }
		inline const IndexBuffer& GetIndexBuffer() const { return *mIndexBuffer; }

		inline const std::vector<VertexData>& GetVerticesData() const { return mVerticesData; }
		inline const std::vector<IndexData>& GetIndicesData() const { return mIndicesData; }

		inline const std::string& GetName() const { return mName; }
		inline void SetName(const char* name) { mName = name; }

		inline RenderConfigs& GetRenderConfigs() { return mRenderConfigs; }
		inline const RenderConfigs& GetRenderConfigs() const { return mRenderConfigs; }
		inline void SetRenderConfigs(const RenderConfigs& configs) { mRenderConfigs = configs; }

		inline bool IsReady() const { return !mVerticesData.empty() && !mIndicesData.empty() && mVertexBuffer && mIndexBuffer; }

	private:
		Device& mDevice;
		RenderConfigs mRenderConfigs;

		std::string mName;

		std::vector<VertexData> mVerticesData;
		std::vector<IndexData> mIndicesData;

		std::unique_ptr<VertexBuffer> mVertexBuffer;
		std::unique_ptr<IndexBuffer> mIndexBuffer;
	};

} // namespace erm
