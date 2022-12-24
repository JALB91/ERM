#pragma once

#include "erm/rendering/buffers/BufferHandle.h"
#include "erm/rendering/data_structs/PipelineConfigs.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

namespace erm {
class Mesh;
}

namespace erm {

class GPUMesh
{
public:
	GPUMesh(
		const Mesh& mesh,
		const RenderConfigs& renderConfigs = RenderConfigs::DEFAULT,
		const PipelineConfigs& pipelineConfigs = PipelineConfigs::DEFAULT);
	virtual ~GPUMesh() = default;

	GPUMesh(GPUMesh&& other) noexcept;
	GPUMesh(const GPUMesh&) = delete;

	GPUMesh& operator=(GPUMesh&& other) = delete;
	GPUMesh& operator=(const GPUMesh&) = delete;
	
	inline const Mesh& GetMesh() const { return mMesh; }
	
	inline const RenderConfigs& GetRenderConfigs() const { return mRenderConfigs; }
	inline RenderConfigs& GetRenderConfigs() { return mRenderConfigs; }

	inline const PipelineConfigs& GetPipelineConfigs() const { return mPipelineConfigs; }
	inline PipelineConfigs& GetPipelineConfigs() { return mPipelineConfigs; }

	inline const BufferHandle& GetVertBufferHandle() const { return mVertBufferHandle; }
	inline void SetVertBufferHandle(BufferHandle&& handle) { mVertBufferHandle = std::move(handle); }

	inline const BufferHandle& GetIndBufferHandle() const { return mIndBufferHandle; }
	inline void SetIndBufferHandle(BufferHandle&& handle) { mIndBufferHandle = std::move(handle); }

protected:
	const Mesh& mMesh;
	RenderConfigs mRenderConfigs;
	PipelineConfigs mPipelineConfigs;
	BufferHandle mVertBufferHandle;
	BufferHandle mIndBufferHandle;
	
};

} // namespace erm
