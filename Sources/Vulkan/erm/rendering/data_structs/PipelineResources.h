#pragma once

#include "erm/rendering/data_structs/PipelineConfigs.h"
#include "erm/rendering/data_structs/PipelineData.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {
class Device;
class Engine;
class IWindow;
class Renderer;
struct RenderData;
} // namespace erm

namespace erm {

class PipelineResources
{
public:
	PipelineResources(
		Engine& engine,
		const vk::RenderPass* renderPass,
		const vk::DescriptorPool* descriptorPool,
		const PipelineConfigs& pipelineConfigs);
	~PipelineResources();

	PipelineResources(PipelineResources&&) = delete;
	PipelineResources& operator=(PipelineResources&&) = delete;

	PipelineResources(const PipelineResources&) = delete;
	PipelineResources& operator=(const PipelineResources&) = delete;

	void Refresh();

	void UpdateResources(vk::CommandBuffer& cmd, RenderData& renderData, uint32_t imageIndex);
	void UpdateCommandBuffer(vk::CommandBuffer& cmd, RenderData& renderData, uint32_t imageIndex);
	void PostDraw();

	inline const PipelineConfigs& GetPipelineConfigs() const { return mPipelineConfigs; }

private:
	void CreatePipeline();
	PipelineData& GetOrCreatePipelineData(RenderData& renderData);

	Device& mDevice;
	IWindow& mWindow;
	Renderer& mRenderer;
	const vk::RenderPass* mRenderPass;
	const vk::DescriptorPool* mDescriptorPool;

	const PipelineConfigs mPipelineConfigs;
	vk::UniquePipelineLayout mPipelineLayout;
	vk::UniquePipeline mPipeline;

	vk::UniqueDescriptorSetLayout mEmptySetLayout;
	vk::UniqueDescriptorSet mEmptySet;
	std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;
	std::vector<PipelineData> mData;
};

} // namespace erm
