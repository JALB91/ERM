#include "erm/rendering/data_structs/RenderConfigs.h"

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

	RenderConfigs::RenderConfigs(const SubpassData& subpassData)
		: mSubpassData(subpassData)
	{}

	bool RenderConfigs::operator==(const RenderConfigs& other) const
	{
		return 
			IsRenderPassLevelCompatible(other) &&
			IsPipelineLevelCompatible(other) &&
			IsBindingLevelCompatible(other);
	}

	bool RenderConfigs::operator!=(const RenderConfigs& other) const
	{
		return !(*this == other);
	}

	bool RenderConfigs::IsRenderPassLevelCompatible(const RenderConfigs& other) const
	{
		return IsSubpassCompatible(other);
	}

	bool RenderConfigs::IsSubpassCompatible(const RenderConfigs& other) const
	{
		return mSubpassData == other.mSubpassData;
	}

} // namespace erm
