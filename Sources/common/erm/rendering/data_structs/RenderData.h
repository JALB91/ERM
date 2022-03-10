#pragma once

#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <optional>
#include <vector>

namespace erm {
class Mesh;
}

namespace erm {

struct RenderData : public IRenderData
{
	RenderData(
		const RenderConfigs& renderConfigs = RenderConfigs::DEFAULT_RENDER_CONFIGS,
		const PipelineConfigs& pipelineConfigs = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS)
		: IRenderData(pipelineConfigs)
		, mRenderConfigs(renderConfigs)
	{}

	RenderData(RenderData&& other) noexcept
		: IRenderData(std::move(other))
		, mMeshes(std::move(other.mMeshes))
		, mRenderingId(std::move(other.mRenderingId))
		, mBindingId(std::move(other.mBindingId))
		, mRenderConfigs(other.mRenderConfigs)
	{}

	RenderData& operator=(RenderData&& other) noexcept
	{
		if (this == &other)
			return *this;

		IRenderData::operator=(std::move(other));
		mMeshes = std::move(other.mMeshes);
		mRenderingId = std::move(other.mRenderingId);
		mBindingId = std::move(other.mBindingId);
		mRenderConfigs = std::move(other.mRenderConfigs);

		return *this;
	}

	RenderData(const RenderData&) = delete;
	RenderData& operator=(const RenderData&) = delete;

	inline bool IsCompatible(const RenderConfigs& renderConfigs, const PipelineConfigs& pipelineConfigs) const
	{
		return 
			mRenderConfigs.IsRenderPassLevelCompatible(renderConfigs) && 
			mPipelineConfigs.IsPipelineLevelCompatible(pipelineConfigs) && 
			mPipelineConfigs.IsBindingLevelCompatible(pipelineConfigs);
	}

	inline bool HasMesh(const Mesh* mesh) const
	{
		return std::find(mMeshes.cbegin(), mMeshes.cend(), mesh) != mMeshes.cend();
	}

	std::vector<const Mesh*> mMeshes;
	std::optional<uint32_t> mRenderingId;
	std::optional<uint32_t> mBindingId;
	RenderConfigs mRenderConfigs;
};

} // namespace erm
