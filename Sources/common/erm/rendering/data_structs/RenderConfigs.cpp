#include "erm/rendering/data_structs/RenderConfigs.h"

#include "erm/utils/Utils.h"

namespace erm {

	const RenderConfigs RenderConfigs::MODELS_RENDER_CONFIGS {
		SubpassData {
			AttachmentData(
				AttachmentLoadOp::CLEAR,
				AttachmentStoreOp::STORE,
				ImageLayout::UNDEFINED,
				ImageLayout::COLOR_ATTACHMENT_OPTIMAL),
			AttachmentData(
				AttachmentLoadOp::CLEAR,
				AttachmentStoreOp::DONT_CARE,
				ImageLayout::UNDEFINED,
				ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL)}};

	RenderConfigs::RenderConfigs(
		const SubpassData& subpassData,
		std::optional<BoundingBox2D> normViewport /*= {}*/,
		std::optional<bool> depthTestEnabled /*= {}*/,
		std::optional<bool> depthWriteEnabled /*= {}*/,
		std::optional<DepthFunction> depthFunction /*= {}*/,
		std::optional<bool> blendEnabled /*= {}*/,
		std::optional<BlendFunction> blendFunction /*= {}*/,
		std::optional<CullMode> cullMode /*= {}*/,
		std::optional<FrontFace> frontFace /*= {}*/,
		std::optional<PolygonMode> polygonMode /*= {}*/,
		std::optional<DrawMode> drawMode /*= {}*/,
		ShaderProgram* shaderProgram /*= nullptr*/,
		PBMaterial* pBMaterial /*= nullptr*/,
		Material* material /*= nullptr*/,
		Texture* diffuseMap /*= nullptr*/,
		Texture* normalMap /*= nullptr*/,
		Texture* specularMap /*= nullptr*/)
		: mSubpassData(subpassData)
		, mNormViewport(normViewport)
		, mDepthTestEnabled(depthTestEnabled)
		, mDepthWriteEnabled(depthWriteEnabled)
		, mDepthFunction(depthFunction)
		, mBlendEnabled(blendEnabled)
		, mBlendFunction(blendFunction)
		, mCullMode(cullMode)
		, mFrontFace(frontFace)
		, mPolygonMode(polygonMode)
		, mDrawMode(drawMode)
		, mShaderProgram(shaderProgram)
		, mPBMaterial(pBMaterial)
		, mMaterial(material)
		, mDiffuseMap(diffuseMap)
		, mNormalMap(normalMap)
		, mSpecularMap(specularMap)
	{}

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

	bool RenderConfigs::IsRenderPassLevelCompatible(const RenderConfigs& other) const
	{
		return IsSubpassCompatible(other);
	}

	bool RenderConfigs::IsPipelineLevelCompatible(const RenderConfigs& other) const
	{
		return IsShaderCompatible(other) &&
			IsBlendCompatible(other) &&
			IsDepthCompatible(other) &&
			IsCullCompatible(other) &&
			IsDrawModeCompatible(other) &&
			IsPolyModeCompatible(other);
	}

	bool RenderConfigs::IsResourcesBindingCompatible(const RenderConfigs& other) const
	{
		return IsMaterialCompatible(other) && AreTexturesCompatible(other);
	}

	bool RenderConfigs::IsSubpassCompatible(const RenderConfigs& other) const
	{
		return mSubpassData == other.mSubpassData;
	}

	bool RenderConfigs::IsBlendCompatible(const RenderConfigs& other) const
	{
		return (!mBlendEnabled && !other.mBlendEnabled) ||
			(mBlendEnabled == other.mBlendEnabled && mBlendFunction == other.mBlendFunction);
	}

	bool RenderConfigs::IsDepthCompatible(const RenderConfigs& other) const
	{
		return ((!mDepthTestEnabled && !other.mDepthTestEnabled) || (mDepthTestEnabled == other.mDepthTestEnabled && mDepthFunction == other.mDepthFunction)) &&
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
		return mPBMaterial == other.mPBMaterial && mMaterial == other.mMaterial;
	}

	bool RenderConfigs::AreTexturesCompatible(const RenderConfigs& other) const
	{
		return mDiffuseMap == other.mDiffuseMap && mNormalMap == other.mNormalMap && mSpecularMap == other.mSpecularMap;
	}

} // namespace erm
