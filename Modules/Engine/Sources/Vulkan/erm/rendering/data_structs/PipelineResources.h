#pragma once

#include "erm/rendering/data_structs/PipelineConfigs.h"
#include "erm/rendering/data_structs/PipelineData.h"

#include <vulkan/vulkan.hpp>

#include <unordered_map>
#include <vector>

namespace erm {
class Device;
class IWindow;
class Renderer;
struct RenderData;
} // namespace erm

namespace erm {

class PipelineResources
{
public:
	PipelineResources(
		Device& device,
		IWindow& window,
		Renderer& renderer,
		const vk::RenderPass* renderPass,
		const vk::DescriptorPool* descriptorPool,
		const PipelineConfigs& pipelineConfigs);
	~PipelineResources();

	PipelineResources(PipelineResources&&) noexcept = default;
	PipelineResources& operator=(PipelineResources&&) noexcept;

	PipelineResources(const PipelineResources&) = delete;
	PipelineResources& operator=(const PipelineResources&) = delete;
	
	inline uint32_t GetUntouchedFrames() const { return mUntouchedFrames; }

	void Refresh();

	void UpdateResources(vk::CommandBuffer& cmd, RenderData& renderData);
	void UpdateCommandBuffer(vk::CommandBuffer& cmd, RenderData& renderData);

	inline const PipelineConfigs& GetPipelineConfigs() const { return mPipelineConfigs; }

private:
	void CreatePipeline();
	PipelineData& GetOrCreatePipelineData(RenderData& renderData);

	Device& mDevice;
	IWindow& mWindow;
	Renderer& mRenderer;
	const vk::RenderPass* mRenderPass;
	const vk::DescriptorPool* mDescriptorPool;

	PipelineConfigs mPipelineConfigs;
	vk::UniquePipelineLayout mPipelineLayout;
	vk::UniquePipeline mPipeline;

	vk::UniqueDescriptorSetLayout mEmptySetLayout;
	vk::UniqueDescriptorSet mEmptySet;
	std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;
	std::unordered_map<uint32_t, PipelineData> mPipelineData;
	uint32_t mUntouchedFrames;
};

} // namespace erm
