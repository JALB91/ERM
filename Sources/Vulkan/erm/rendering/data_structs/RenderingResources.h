#pragma once

#include "erm/rendering/data_structs/PipelineResources.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <vector>

namespace erm {
class Device;
class Engine;
class Renderer;
struct AttachmentData;
struct PipelineConfigs;
struct RenderData;
} // namespace erm

namespace erm {

class RenderingResources
{
public:
	RenderingResources(Engine& engine);

	RenderingResources(RenderingResources&& other) = delete;
	RenderingResources(const RenderingResources&) = delete;

	RenderingResources& operator=(RenderingResources&&) = delete;
	RenderingResources& operator=(const RenderingResources&) = delete;

	void Refresh();
	vk::CommandBuffer UpdateCommandBuffer(std::vector<RenderData*>& renderData, uint32_t imageIndex);
	void PostDraw();

private:
	vk::AttachmentDescription CreateAttachmentDescription(const erm::AttachmentData& data, vk::Format format) const;
	PipelineResources& GetOrCreatePipelineResources(const PipelineConfigs& pipelineConfigs);

	void Reload();
	void Cleanup();

	void CreateRenderPass();
	void CreateFramebuffers();
	void CreateDescriptorPool();
	void CreateCommandBuffers();

	Engine& mEngine;
	Device& mDevice;
	Renderer& mRenderer;
	vk::UniqueRenderPass mRenderPass;
	std::vector<vk::UniqueFramebuffer> mSwapChainFramebuffers;
	vk::UniqueDescriptorPool mDescriptorPool;
	std::vector<std::unique_ptr<PipelineResources>> mPipelineResources;
	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;
};

} // namespace erm
