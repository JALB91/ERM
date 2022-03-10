#pragma once

#include "erm/rendering/renderer/IRenderer.h"

#include <memory>
#include <vector>

namespace erm {
class RenderingResources;
struct RenderData;
#ifdef ERM_RAY_TRACING_ENABLED
class RTRenderingResources;
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
#ifdef ERM_RAY_TRACING_ENABLED
	void SubmitRTRenderData(RTRenderData& data);
#endif

private:
	void RecreateSwapChain() override;
	void CreateCommandBuffers();

	vk::CommandBuffer& RetrieveCommandBuffer();

	std::map<uint32_t, std::pair<std::unique_ptr<RenderingResources>, std::vector<RenderData*>>> mRenderingMap;

#ifdef ERM_RAY_TRACING_ENABLED
	std::unique_ptr<RTRenderingResources> mRTRenderingResources;
	RTRenderData* mRTRenderData;
#endif

	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;
};

} // namespace erm
