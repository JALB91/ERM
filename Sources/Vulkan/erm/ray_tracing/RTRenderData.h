#pragma once

#include "erm/ray_tracing/RTBlas.h"
#include "erm/ray_tracing/RTRenderConfigs.h"
#include "erm/rendering/data_structs/IRenderData.h"

namespace erm {

	struct RTRenderData : public IRenderData
	{
		RTRenderData(const RTRenderConfigs& renderConfigs = {})
			: mRenderConfigs(renderConfigs)
			, mTransform(glm::identity<math::mat4>())
		{}

		RTRenderData(RTRenderData&& other)
			: IRenderData(std::move(other))
			, mRenderConfigs(other.mRenderConfigs)
			, mTransform(std::move(other.mTransform))
			, mBlas(other.mBlas)
		{}

		RTRenderData& operator=(RTRenderData&& other)
		{
			mUbos = std::move(other.mUbos);
			mRenderConfigs = other.mRenderConfigs;
			mTransform = std::move(other.mTransform);
			mBlas = other.mBlas;

			return *this;
		}

		RTRenderConfigs mRenderConfigs;
		math::mat4 mTransform;
		RTBlas* mBlas;
	};

} // namespace erm
