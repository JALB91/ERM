#pragma once

#include "erm/rendering/buffers/IndexData.h"
#include "erm/rendering/buffers/VertexData.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <string>
#include <vector>

namespace erm {

	class Mesh
	{
	public:
		Mesh(
			std::vector<VertexData>&& vertices,
			std::vector<IndexData>&& indices,
			const RenderConfigs& configs = RenderConfigs::MODELS_RENDER_CONFIGS,
			const char* name = "");
		~Mesh();

		Mesh(Mesh&& other);
		Mesh(const Mesh&) = delete;

		Mesh& operator=(Mesh&& other);
		Mesh& operator=(const Mesh&) = delete;

		inline const std::vector<VertexData>& GetVerticesData() const { return mVerticesData; }
		inline const std::vector<IndexData>& GetIndicesData() const { return mIndicesData; }

		inline const std::string& GetName() const { return mName; }
		inline void SetName(const char* name) { mName = name; }

		inline RenderConfigs& GetRenderConfigs() { return mRenderConfigs; }
		inline const RenderConfigs& GetRenderConfigs() const { return mRenderConfigs; }
		inline void SetRenderConfigs(const RenderConfigs& configs) { mRenderConfigs = configs; }

	private:
		std::vector<VertexData> mVerticesData;
		std::vector<IndexData> mIndicesData;
		RenderConfigs mRenderConfigs;
		std::string mName;
	};

} // namespace erm
