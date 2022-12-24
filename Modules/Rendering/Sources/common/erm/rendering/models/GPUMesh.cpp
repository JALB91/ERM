#include "erm/rendering/models/GPUMesh.h"

namespace erm {

GPUMesh::GPUMesh(
	const Mesh& mesh,
	const RenderConfigs& renderConfigs /* = RenderConfigs::DEFAULT*/,
	const PipelineConfigs& pipelineConfigs /* = PipelineConfigs::DEFAULT*/
	)
	: mMesh(mesh)
	, mRenderConfigs(renderConfigs)
	, mPipelineConfigs(pipelineConfigs)
	, mVertBufferHandle(nullptr, {})
	, mIndBufferHandle(nullptr, {})
{}

GPUMesh::GPUMesh(GPUMesh&& other) noexcept
	: mMesh(other.mMesh)
	, mRenderConfigs(std::move(other.mRenderConfigs))
	, mPipelineConfigs(std::move(other.mPipelineConfigs))
	, mVertBufferHandle(std::move(other.mVertBufferHandle))
	, mIndBufferHandle(std::move(other.mIndBufferHandle))
{}

} // namespace erm
