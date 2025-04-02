#pragma once

#include "erm/rendering/data_structs/PipelineConfigs.h"
#include "erm/rendering/data_structs/PipelineData.h"

#include <erm/math/Types.h>

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
	
	inline u32 getUntouchedFrames() const { return mUntouchedFrames; }

	void refresh();

	void updateResources(vk::CommandBuffer& cmd, RenderData& renderData);
	void updateCommandBuffer(vk::CommandBuffer& cmd, RenderData& renderData);

	inline const PipelineConfigs& getPipelineConfigs() const { return mPipelineConfigs; }

private:
	void createPipeline();
	PipelineData& getOrCreatePipelineData(RenderData& renderData);

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
	std::unordered_map<u32, PipelineData> mPipelineData;
	u32 mUntouchedFrames;
};

} // namespace erm
