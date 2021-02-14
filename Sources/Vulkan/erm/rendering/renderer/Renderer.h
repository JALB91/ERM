#pragma once

#include "erm/rendering/data_structs/RenderConfigs.h"
#include "erm/rendering/renderer/IRenderer.h"

#include <map>
#include <memory>
#include <vector>

namespace erm {
	class RenderingResources;
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

		void SubmitRenderData(RenderData& data) override;

	private:
		using FramesData = std::map<std::unique_ptr<RenderingResources>, std::vector<RenderData*>>;

	private:
		std::vector<vk::CommandBuffer> RetrieveCommandBuffers();

		FramesData::value_type& GetOrCreateFramesData(const RenderConfigs& renderConfigs);

		FramesData mFramesDatas;
	};

} // namespace erm
