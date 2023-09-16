#include "erm/rendering/data_structs/PipelineConfigs.h"

namespace erm {

const PipelineConfigs PipelineConfigs::DEFAULT = {};

bool PipelineConfigs::IsPipelineLevelCompatible(const PipelineConfigs& other) const
{
	return IsShaderCompatible(other) &&
		IsBlendCompatible(other) &&
		IsDepthCompatible(other) &&
		IsCullCompatible(other) &&
		IsDrawModeCompatible(other) &&
		IsPolyModeCompatible(other);
}

bool PipelineConfigs::IsShaderCompatible(const PipelineConfigs& other) const
{
	return mShaderProgram == other.mShaderProgram;
}

bool PipelineConfigs::IsBlendCompatible(const PipelineConfigs& other) const
{
	return (!mBlendEnabled && !other.mBlendEnabled) ||
		(mBlendEnabled == other.mBlendEnabled && mBlendFunction == other.mBlendFunction);
}

bool PipelineConfigs::IsCullCompatible(const PipelineConfigs& other) const
{
	return (mCullMode == other.mCullMode && mFrontFace == other.mFrontFace);
}

bool PipelineConfigs::IsDepthCompatible(const PipelineConfigs& other) const
{
	return ((!mDepthTestEnabled && !other.mDepthTestEnabled) || (mDepthTestEnabled == other.mDepthTestEnabled && mDepthFunction == other.mDepthFunction)) &&
		mDepthWriteEnabled == other.mDepthWriteEnabled;
}

bool PipelineConfigs::IsDrawModeCompatible(const PipelineConfigs& other) const
{
	return mDrawMode == other.mDrawMode;
}

bool PipelineConfigs::IsPolyModeCompatible(const PipelineConfigs& other) const
{
	return mPolygonMode == other.mPolygonMode;
}

} // namespace erm
