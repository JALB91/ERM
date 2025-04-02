#pragma once

#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <erm/math/Types.h>

#include <erm/utils/StringID.h>

#include <optional>
#include <vector>

namespace erm {

struct RenderData : public IRenderData
{
	RenderData(
		const RenderConfigs& renderConfigs = RenderConfigs::DEFAULT,
		const PipelineConfigs& pipelineConfigs = PipelineConfigs::DEFAULT)
		: IRenderData(pipelineConfigs)
		, mRenderConfigs(renderConfigs)
	{}

	RenderData(RenderData&& other) noexcept
		: IRenderData(std::move(other))
		, mMeshes(std::move(other.mMeshes))
		, mBindingId(std::move(other.mBindingId))
		, mRenderConfigs(other.mRenderConfigs)
	{}

	RenderData& operator=(RenderData&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}

		IRenderData::operator=(std::move(other));
		mMeshes = std::move(other.mMeshes);
		mBindingId = std::move(other.mBindingId);
		mRenderConfigs = std::move(other.mRenderConfigs);

		return *this;
	}

	RenderData(const RenderData&) = delete;
	RenderData& operator=(const RenderData&) = delete;

	inline bool isCompatible(const RenderConfigs& renderConfigs, const PipelineConfigs& pipelineConfigs) const
	{
		return 
			mRenderConfigs.isRenderPassLevelCompatible(renderConfigs) && 
			mPipelineConfigs.isPipelineLevelCompatible(pipelineConfigs) && 
			mPipelineConfigs.isBindingLevelCompatible(pipelineConfigs);
	}

	inline bool hasMesh(StringID mesh) const
	{
		return std::find(mMeshes.cbegin(), mMeshes.cend(), mesh) != mMeshes.cend();
	}

	std::vector<StringID> mMeshes;
	std::optional<u32> mBindingId;
	RenderConfigs mRenderConfigs;
};

} // namespace erm
