#include "erm/rendering/data_structs/RenderConfigs.h"

namespace erm {

const RenderConfigs RenderConfigs::DEFAULT (
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
	vec4(0.0f, 0.0f, 0.0f, 1.0f)
);

RenderConfigs::RenderConfigs(SubpassData&& subpassData, vec4&& clearColor)
	: mSubpassData(std::move(subpassData))
	, mClearColor(std::move(clearColor))
{}

bool RenderConfigs::isRenderPassLevelCompatible(const RenderConfigs& other) const
{
	return mSubpassData == other.mSubpassData;
}

} // namespace erm
