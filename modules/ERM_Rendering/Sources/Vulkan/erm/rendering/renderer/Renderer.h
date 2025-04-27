#pragma once

#include "erm/rendering/renderer/IRenderer.h"

#include <refl.hpp>

#include <map>
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
	Renderer(Window& window, Device& device);
	~Renderer();

	void preRender() override;
	void render() override;
	void postRender() override;

	void submitRenderData(RenderData& data);
#ifdef ERM_RAY_TRACING_ENABLED
	void submitRTRenderData(RTRenderData& data);
#endif

private:
	void recreateSwapChain() override;
	void createCommandBuffers();

	vk::CommandBuffer& retrieveCommandBuffer();

	std::map<std::unique_ptr<RenderingResources>, std::vector<RenderData*>> mRenderingMap;

#ifdef ERM_RAY_TRACING_ENABLED
	std::unique_ptr<RTRenderingResources> mRTRenderingResources;
	RTRenderData* mRTRenderData;
#endif

	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;
};

} // namespace erm

REFL_AUTO(type(erm::Renderer));
