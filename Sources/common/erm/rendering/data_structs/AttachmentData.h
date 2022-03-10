#pragma once

#include "erm/rendering/enums/AttachmentLoadOp.h"
#include "erm/rendering/enums/AttachmentStoreOp.h"
#include "erm/rendering/enums/FrameBufferType.h"
#include "erm/rendering/enums/ImageLayout.h"

#include "erm/utils/Utils.h"

namespace erm {

struct AttachmentData
{
	AttachmentData(
		FrameBufferType frameBufferType,
		AttachmentLoadOp loadOp,
		AttachmentStoreOp storeOp,
		ImageLayout initialLayout,
		ImageLayout finalLayout,
		std::optional<AttachmentLoadOp> stencilLoadOp = {},
		std::optional<AttachmentStoreOp> stencilStoreOp = {})
		: mFrameBufferType(frameBufferType)
		, mLoadOp(loadOp)
		, mStoreOp(storeOp)
		, mInitialLayout(initialLayout)
		, mFinalLayout(finalLayout)
		, mStencilLoadOp(stencilLoadOp)
		, mStencilStoreOp(stencilStoreOp)
	{}

	inline bool operator==(const AttachmentData& other) const
	{
		return mFrameBufferType == other.mFrameBufferType &&
			mLoadOp == other.mLoadOp &&
			mStoreOp == other.mStoreOp &&
			mInitialLayout == other.mInitialLayout &&
			mFinalLayout == other.mFinalLayout &&
			mStencilLoadOp == other.mStencilLoadOp &&
			mStencilStoreOp == other.mStencilStoreOp;
	}

	FrameBufferType mFrameBufferType;
	AttachmentLoadOp mLoadOp;
	AttachmentStoreOp mStoreOp;
	ImageLayout mInitialLayout;
	ImageLayout mFinalLayout;
	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(StencilLoadOp, AttachmentLoadOp, AttachmentLoadOp::DONT_CARE);
	ERM_DECL_SET_GET_OPT_WITH_DEFAULT(StencilStoreOp, AttachmentStoreOp, AttachmentStoreOp::DONT_CARE);
};

} // namespace erm
