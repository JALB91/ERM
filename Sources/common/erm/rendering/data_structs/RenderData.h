#pragma once

#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vector>

namespace erm {
class Mesh;
}

namespace erm {

struct RenderData : public IRenderData
{
	RenderData(const RenderConfigs& renderConfigs)
		: mRenderConfigs(renderConfigs)
	{}
	RenderData(const SubpassData& subpassData)
		: mRenderConfigs(subpassData)
	{}

	RenderData(RenderData&& other)
		: IRenderData(std::move(other))
		, mRenderConfigs(other.mRenderConfigs)
		, mMeshes(std::move(other.mMeshes))
	{}

	RenderData& operator=(RenderData&& other)
	{
		mRenderConfigs = other.mRenderConfigs;
		mUbos = std::move(other.mUbos);
		mMeshes = std::move(other.mMeshes);

		return *this;
	}

	RenderData(const RenderData&) = delete;
	RenderData& operator=(const RenderData&) = delete;

	inline bool HasMesh(const Mesh* mesh) const
	{
		return std::find(mMeshes.cbegin(), mMeshes.cend(), mesh) != mMeshes.cend();
	}

	RenderConfigs mRenderConfigs;
	std::vector<const Mesh*> mMeshes;
};

} // namespace erm
