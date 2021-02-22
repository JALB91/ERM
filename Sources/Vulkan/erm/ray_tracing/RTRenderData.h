#pragma once

#include "erm/ray_tracing/RTRenderConfigs.h"
#include "erm/rendering/data_structs/IRenderData.h"

#include <vector>

namespace erm {
	class RTBlas;
}

namespace erm {

	struct RTRenderData : public IRenderData
	{
		RTRenderData(const RTRenderConfigs& renderConfigs = {})
			: mRenderConfigs(renderConfigs)
		{}

		RTRenderData(RTRenderData&& other)
			: IRenderData(std::move(other))
			, mRenderConfigs(other.mRenderConfigs)
			, mBlas(other.mBlas)
		{}

		RTRenderData& operator=(RTRenderData&& other)
		{
			mUbos = std::move(other.mUbos);
			mRenderConfigs = other.mRenderConfigs;
			mBlas = other.mBlas;

			return *this;
		}

		RTRenderConfigs mRenderConfigs;
		RTBlas* mBlas;
	};

} // namespace erm
