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

	std::vector<vk::CommandBuffer> RetrieveCommandBuffers();

	std::unique_ptr<RenderingResources> mRenderingResources;
	std::vector<RenderData*> mRenderData;

#ifdef ERM_RAY_TRACING_ENABLED
	std::unique_ptr<RTRenderingResources> mRTRenderingResources;
	RTRenderData* mRTRenderData;
#endif
};

} // namespace erm
