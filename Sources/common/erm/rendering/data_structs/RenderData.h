#pragma once

#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/data_structs/PipelineConfigs.h"

#include <optional>
#include <vector>

namespace erm {
class Mesh;
}

namespace erm {

struct RenderData : public IRenderData
{
	RenderData(const PipelineConfigs& pipelineConfigs)
		: mPipelineConfigs(pipelineConfigs)
	{}

	RenderData(RenderData&& other)
		: IRenderData(std::move(other))
		, mPipelineConfigs(other.mPipelineConfigs)
		, mMeshes(std::move(other.mMeshes))
		, mRenderingId(std::move(other.mRenderingId))
	{}

	RenderData& operator=(RenderData&& other)
	{
		mPipelineConfigs = other.mPipelineConfigs;
		mUbos = std::move(other.mUbos);
		mMeshes = std::move(other.mMeshes);
		mRenderingId = std::move(other.mRenderingId);

		return *this;
	}

	RenderData(const RenderData&) = delete;
	RenderData& operator=(const RenderData&) = delete;

	inline bool HasMesh(const Mesh* mesh) const
	{
		return std::find(mMeshes.cbegin(), mMeshes.cend(), mesh) != mMeshes.cend();
	}

	PipelineConfigs mPipelineConfigs;
	std::vector<const Mesh*> mMeshes;
	std::optional<uint32_t> mRenderingId;
};

} // namespace erm
