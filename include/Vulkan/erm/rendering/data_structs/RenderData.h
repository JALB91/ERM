#pragma once

#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vector>

namespace erm {
	class Mesh;
}

namespace erm {

	struct RenderData
	{
		RenderData(const RenderConfigs& renderConfigs)
			: mRenderConfigs(renderConfigs)
		{}
		RenderData()
		{}

		RenderConfigs mRenderConfigs;

		UniformBuffer mUniformBuffer;
		std::vector<Mesh*> mMehses;
	};

} // namespace erm
