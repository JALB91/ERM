#pragma once

#include "erm/rendering/data_structs/RenderConfigs.h"
#include "erm/rendering/renderer/IRenderer.h"
// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include "erm/ray_tracing/RTRenderingResources.h"
#endif
// clang-format on

#include <map>
#include <memory>
#include <vector>

namespace erm {
	class RenderingResources;
	struct RenderData;
#ifdef ERM_RAY_TRACING_ENABLED
	struct RTRenderData;
#endif
} // namespace erm

namespace erm {

	class Renderer : public IRenderer
	{
	public:
		Renderer(Engine& engine);
		~Renderer();

		void OnPreRender() override;
		void OnRender() override;
		void OnPostRender() override;

		void SubmitRenderData(RenderData& data);

	private:
		using RasterData = std::map<std::unique_ptr<RenderingResources>, std::vector<RenderData*>>;

	private:
		std::vector<vk::CommandBuffer> RetrieveCommandBuffers();

		RasterData::value_type& GetOrCreateFramesData(const RenderConfigs& renderConfigs);

		RasterData mRasterData;

#ifdef ERM_RAY_TRACING_ENABLED
	public:
		void SubmitRTRenderData(RTRenderData& data);

	private:
		std::vector<std::unique_ptr<RTRenderingResources>> mRTRenderingResources;
		std::map<RTRenderingResources*, std::vector<RTRenderData*>> mRTRenderData;
#endif
	};

} // namespace erm
