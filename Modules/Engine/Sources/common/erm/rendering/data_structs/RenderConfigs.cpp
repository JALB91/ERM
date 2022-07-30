#include "erm/rendering/data_structs/RenderConfigs.h"

namespace erm {

const RenderConfigs RenderConfigs::DEFAULT_RENDER_CONFIGS {
	SubpassData {
		AttachmentData(
			FrameBufferType::FRAME_1,
			AttachmentLoadOp::CLEAR,
			AttachmentStoreOp::STORE,
			ImageLayout::UNDEFINED,
			ImageLayout::GENERAL),
		AttachmentData(
			FrameBufferType::DEPTH,
			AttachmentLoadOp::CLEAR,
			AttachmentStoreOp::STORE,
			ImageLayout::UNDEFINED,
			ImageLayout::GENERAL)}};

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
	return mSubpassData == other.mSubpassData;
}

} // namespace erm
