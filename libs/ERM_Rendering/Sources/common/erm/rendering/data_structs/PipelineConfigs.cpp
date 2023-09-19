#include "erm/rendering/data_structs/PipelineConfigs.h"

namespace erm {

const PipelineConfigs PipelineConfigs::DEFAULT = {};

bool PipelineConfigs::isPipelineLevelCompatible(const PipelineConfigs& other) const
{
	return isShaderCompatible(other) &&
		isBlendCompatible(other) &&
		isDepthCompatible(other) &&
		isCullCompatible(other) &&
		isDrawModeCompatible(other) &&
		isPolyModeCompatible(other);
}

bool PipelineConfigs::isShaderCompatible(const PipelineConfigs& other) const
{
	return mShaderProgram == other.mShaderProgram;
}

bool PipelineConfigs::isBlendCompatible(const PipelineConfigs& other) const
{
	return (!mBlendEnabled && !other.mBlendEnabled) ||
		(mBlendEnabled == other.mBlendEnabled && mBlendFunction == other.mBlendFunction);
}

bool PipelineConfigs::isCullCompatible(const PipelineConfigs& other) const
{
	return (mCullMode == other.mCullMode && mFrontFace == other.mFrontFace);
}

bool PipelineConfigs::isDepthCompatible(const PipelineConfigs& other) const
{
	return ((!mDepthTestEnabled && !other.mDepthTestEnabled) || (mDepthTestEnabled == other.mDepthTestEnabled && mDepthFunction == other.mDepthFunction)) &&
		mDepthWriteEnabled == other.mDepthWriteEnabled;
}

bool PipelineConfigs::isDrawModeCompatible(const PipelineConfigs& other) const
{
	return mDrawMode == other.mDrawMode;
}

bool PipelineConfigs::isPolyModeCompatible(const PipelineConfigs& other) const
{
	return mPolygonMode == other.mPolygonMode;
}

} // namespace erm
