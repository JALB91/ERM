#pragma once

#include "erm/rendering/data_structs/AttachmentData.h"

#include <vector>

namespace erm {

struct SubpassData
{
	SubpassData(AttachmentData colorAttachment, std::optional<AttachmentData> depthAttachment = {})
		: mColorAttachment(colorAttachment)
		, mDepthAttachment(depthAttachment)
	{}

	inline bool operator==(const SubpassData& other) const
	{
		return mColorAttachment == other.mColorAttachment && mDepthAttachment == other.mDepthAttachment;
	}

	AttachmentData mColorAttachment;
	std::optional<AttachmentData> mDepthAttachment;
};

} // namespace erm
