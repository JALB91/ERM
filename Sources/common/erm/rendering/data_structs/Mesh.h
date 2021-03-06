#pragma once

#include "erm/rendering/buffers/BufferHandle.h"
#include "erm/rendering/data_structs/IndexData.h"
#include "erm/rendering/data_structs/PipelineConfigs.h"
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
	friend class Model;

public:
	Mesh(
		std::vector<VertexData>&& vertices,
		std::vector<IndexData>&& indices,
		const PipelineConfigs& configs = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS,
		const char* name = "");
	~Mesh();

	Mesh(Mesh&& other);
	Mesh(const Mesh&) = delete;

	Mesh& operator=(Mesh&& other);
	Mesh& operator=(const Mesh&) = delete;

	inline const std::vector<VertexData>& GetVerticesData() const { return mVerticesData; }
	inline const std::vector<IndexData>& GetIndicesData() const { return mIndicesData; }

	inline const BufferHandle& GetVertBufferHandle() const { return mVertBuffer; }
	inline const BufferHandle& GetIndBufferHandle() const { return mIndBuffer; }

	inline const std::string& GetName() const { return mName; }
	inline void SetName(const char* name) { mName = name; }

	inline PipelineConfigs& GetPipelineConfigs() { return mPipelineConfigs; }
	inline const PipelineConfigs& GetPipelineConfigs() const { return mPipelineConfigs; }

protected:
	std::vector<VertexData> mVerticesData;
	std::vector<IndexData> mIndicesData;
	PipelineConfigs mPipelineConfigs;
	BufferHandle mVertBuffer;
	BufferHandle mIndBuffer;
	std::string mName;
};

} // namespace erm
