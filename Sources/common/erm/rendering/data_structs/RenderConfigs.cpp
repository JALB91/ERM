#include "erm/rendering/data_structs/RenderConfigs.h"

namespace erm {

const RenderConfigs RenderConfigs::DEFAULT_RENDER_CONFIGS {
	SubpassData {
		AttachmentData(
			AttachmentLoadOp::CLEAR,
			AttachmentStoreOp::STORE,
			ImageLayout::UNDEFINED,
#ifdef ERM_RAY_TRACING_ENABLED
			ImageLayout::GENERAL
#else
			ImageLayout::COLOR_ATTACHMENT_OPTIMAL
#endif
			),
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
	return IsRenderPassLevelCompatible(other);
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
