#pragma once

#include "erm/rendering/buffers/BufferHandle.h"
#include "erm/rendering/data_structs/IndexData.h"
#include "erm/rendering/data_structs/PipelineConfigs.h"
#include "erm/rendering/data_structs/RenderConfigs.h"
#include "erm/rendering/data_structs/VertexData.h"

#include <string>
#include <vector>

namespace erm {
class Model;
}

namespace erm {

class Mesh
{
public:
	Mesh(
		std::vector<VertexData>&& vertices,
		std::vector<IndexData>&& indices,
		const RenderConfigs& renderConfigs = RenderConfigs::DEFAULT_RENDER_CONFIGS,
		const PipelineConfigs& pipelineConfigs = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS,
		const char* name = "");
	~Mesh();

	Mesh(Mesh&& other) noexcept;
	Mesh(const Mesh&) = delete;

	Mesh& operator=(Mesh&& other) noexcept;
	Mesh& operator=(const Mesh&) = delete;

	inline const std::vector<VertexData>& GetVerticesData() const { return mVerticesData; }
	inline const std::vector<IndexData>& GetIndicesData() const { return mIndicesData; }

	inline const BufferHandle& GetVertBufferHandle() const { return mVertBuffer; }
	inline void SetVertBufferHandle(BufferHandle&& handle) { mVertBuffer = std::move(handle); }

	inline const BufferHandle& GetIndBufferHandle() const { return mIndBuffer; }
	inline void SetIndBufferHandle(BufferHandle&& handle) { mIndBuffer = std::move(handle); }

	inline const std::string& GetName() const { return mName; }
	inline void SetName(const char* name) { mName = name; }

	inline const RenderConfigs& GetRenderConfigs() const { return mRenderConfigs; }
	inline RenderConfigs& GetRenderConfigs() { return mRenderConfigs; }

	inline const PipelineConfigs& GetPipelineConfigs() const { return mPipelineConfigs; }
	inline PipelineConfigs& GetPipelineConfigs() { return mPipelineConfigs; }

protected:
	std::vector<VertexData> mVerticesData;
	std::vector<IndexData> mIndicesData;
	RenderConfigs mRenderConfigs;
	PipelineConfigs mPipelineConfigs;
	BufferHandle mVertBuffer;
	BufferHandle mIndBuffer;
	std::string mName;
};

} // namespace erm
