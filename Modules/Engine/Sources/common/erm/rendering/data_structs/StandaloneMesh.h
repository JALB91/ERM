#pragma once

#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/data_structs/Mesh.h"

namespace erm {
class Device;
}

namespace erm {

class StandaloneMesh : public Mesh
{
public:
	StandaloneMesh(Device& device, Mesh&& mesh);
	StandaloneMesh(
		Device& device,
		std::vector<VertexData>&& vertices,
		std::vector<IndexData>&& indices,
		const RenderConfigs& renderConfigs = RenderConfigs::DEFAULT_RENDER_CONFIGS,
		const PipelineConfigs& pipelineConfigs = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS,
		const char* name = "");
	~StandaloneMesh();

	StandaloneMesh(StandaloneMesh&& other) noexcept;
	StandaloneMesh(const StandaloneMesh&) = delete;

	StandaloneMesh& operator=(StandaloneMesh&&) = delete;
	StandaloneMesh& operator=(const StandaloneMesh&) = delete;

	inline const IndexBuffer& GetIndexBuffer() const { return mIndexBuffer; }
	inline const VertexBuffer& GetVertexBuffer() const { return mVertexBuffer; }

private:
	Device& mDevice;
	IndexBuffer mIndexBuffer;
	VertexBuffer mVertexBuffer;
};

} // namespace erm
