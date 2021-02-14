#pragma once

#include "erm/rendering/enums/AttachmentLoadOp.h"
#include "erm/rendering/enums/AttachmentStoreOp.h"
#include "erm/rendering/enums/ImageLayout.h"

#include "erm/utils/Utils.h"

namespace erm {

	struct AttachmentData
	{
		AttachmentData(
			AttachmentLoadOp loadOp,
			AttachmentStoreOp storeOp,
			ImageLayout initialLayout,
			ImageLayout finalLayout,
			std::optional<AttachmentLoadOp> stencilLoadOp = {},
			std::optional<AttachmentStoreOp> stencilStoreOp = {})
			: mLoadOp(loadOp)
			, mStoreOp(storeOp)
			, mInitialLayout(initialLayout)
			, mFinalLayout(finalLayout)
			, mStencilLoadOp(stencilLoadOp)
			, mStencilStoreOp(stencilStoreOp)
		{}

		inline bool operator==(const AttachmentData& other) const
		{
			return mLoadOp == other.mLoadOp &&
				mStoreOp == other.mStoreOp &&
				mInitialLayout == other.mInitialLayout &&
				mFinalLayout == other.mFinalLayout &&
				mStencilLoadOp == other.mStencilLoadOp &&
				mStencilStoreOp == other.mStencilStoreOp;
		}

		AttachmentLoadOp mLoadOp;
		AttachmentStoreOp mStoreOp;
		ImageLayout mInitialLayout;
		ImageLayout mFinalLayout;
		DECL_SET_GET_OPT_WITH_DEFAULT(StencilLoadOp, AttachmentLoadOp, AttachmentLoadOp::DONT_CARE);
		DECL_SET_GET_OPT_WITH_DEFAULT(StencilStoreOp, AttachmentStoreOp, AttachmentStoreOp::DONT_CARE);
	};

} // namespace erm
