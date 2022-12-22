#include "erm/rendering/data_structs/RenderConfigs.h"

namespace erm {

const RenderConfigs RenderConfigs::DEFAULT_RENDER_CONFIGS (
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
			ImageLayout::GENERAL)
	},
	math::vec4(0.0f, 0.0f, 0.0f, 1.0f)
);

RenderConfigs::RenderConfigs(SubpassData&& subpassData, math::vec4&& clearColor)
	: mSubpassData(std::move(subpassData))
	, mClearColor(std::move(clearColor))
{}

bool RenderConfigs::IsRenderPassLevelCompatible(const RenderConfigs& other) const
{
	return mSubpassData == other.mSubpassData;
}

} // namespace erm
