#include "erm/rendering/data_structs/RenderConfigs.h"

namespace erm {

	RenderConfigs::RenderConfigs()
		: mViewport(math::vec2(0.0f, 0.0f), math::vec2(1.0f, 1.0f))
		, mDepthTestEnabled(true)
		, mDepthWriteEnabled(true)
		, mDepthFunction(DepthFunction::LESS)
		, mBlendEnabled(false)
		, mBlendFunction(BlendFunction::ZERO)
		, mCullMode(CullMode::NONE)
		, mFrontFace(FrontFace::CCW)
		, mPolygonMode(PolygonMode::FILL)
		, mDrawMode(DrawMode::TRIANGLES)
		, mShaderProgram(nullptr)
		, mMaterial(nullptr)
		, mTexture(nullptr)
	{
	}

	bool RenderConfigs::operator==(const RenderConfigs& other) const
	{
		return IsRenderPassLevelCompatible(other) &&
			IsPipelineLevelCompatible(other) &&
			IsResourcesBindingCompatible(other);
	}

	bool RenderConfigs::operator!=(const RenderConfigs& other) const
	{
		return !(*this == other);
	}

	bool RenderConfigs::operator<(const RenderConfigs& other) const
	{
		const uint64_t total = static_cast<int>(mDepthFunction) + static_cast<int>(mBlendFunction) + static_cast<int>(mCullMode) + static_cast<int>(mFrontFace) + static_cast<int>(mPolygonMode) + static_cast<int>(mDrawMode);
		const uint64_t otherTotal = static_cast<int>(other.mDepthFunction) + static_cast<int>(other.mBlendFunction) + static_cast<int>(other.mCullMode) + static_cast<int>(other.mFrontFace) + static_cast<int>(other.mPolygonMode) + static_cast<int>(other.mDrawMode);
		return total < otherTotal;
	}

	bool RenderConfigs::IsRenderPassLevelCompatible(const RenderConfigs& other) const
	{
		return IsBlendCompatible(other) &&
			IsDepthCompatible(other) &&
			IsCullCompatible(other) &&
			IsDrawModeCompatible(other) &&
			IsPolyModeCompatible(other);
	}

	bool RenderConfigs::IsPipelineLevelCompatible(const RenderConfigs& other) const
	{
		return IsShaderCompatible(other);
	}

	bool RenderConfigs::IsResourcesBindingCompatible(const RenderConfigs& other) const
	{
		return IsMaterialCompatible(other) &&
			IsTextureCompatible(other);
	}

	bool RenderConfigs::IsBlendCompatible(const RenderConfigs& other) const
	{
		return (!mBlendEnabled && !other.mBlendEnabled) ||
			(mBlendEnabled == other.mBlendEnabled && mBlendFunction == other.mBlendFunction);
	}

	bool RenderConfigs::IsDepthCompatible(const RenderConfigs& other) const
	{
		return ((!mDepthTestEnabled && !other.mDepthTestEnabled) ||
				(mDepthTestEnabled == other.mDepthTestEnabled && mDepthFunction == other.mDepthFunction)) &&
			mDepthWriteEnabled == other.mDepthWriteEnabled;
	}

	bool RenderConfigs::IsCullCompatible(const RenderConfigs& other) const
	{
		return (mCullMode == other.mCullMode && mFrontFace == other.mFrontFace);
	}

	bool RenderConfigs::IsDrawModeCompatible(const RenderConfigs& other) const
	{
		return mDrawMode == other.mDrawMode;
	}

	bool RenderConfigs::IsPolyModeCompatible(const RenderConfigs& other) const
	{
		return mPolygonMode == other.mPolygonMode;
	}

	bool RenderConfigs::IsShaderCompatible(const RenderConfigs& other) const
	{
		return mShaderProgram == other.mShaderProgram;
	}

	bool RenderConfigs::IsMaterialCompatible(const RenderConfigs& other) const
	{
		return (!mMaterial && !other.mMaterial) || (mMaterial && other.mMaterial);
	}

	bool RenderConfigs::IsTextureCompatible(const RenderConfigs& other) const
	{
		return (!mTexture && !other.mTexture) || (mTexture && other.mTexture);
	}

} // namespace erm
