#pragma once

#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <optional>
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

		UniformBufferObject mUBO;
		std::vector<Mesh*> mMehses;
		std::optional<uint32_t> mRenderingId;
	};

} // namespace erm
