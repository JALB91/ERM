#pragma once

#include "erm/rendering/data_structs/PipelineResources.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <vector>

namespace erm {
class Device;
class IWindow;
class Renderer;
struct AttachmentData;
struct PipelineConfigs;
struct RenderData;
} // namespace erm

namespace erm {

class RenderingResources
{
public:
	RenderingResources(
		Device& device,
		IWindow& window,
		Renderer& renderer,
		const RenderConfigs& renderConfigs
	);

	RenderingResources(RenderingResources&& other) = delete;
	RenderingResources(const RenderingResources&) = delete;

	RenderingResources& operator=(RenderingResources&&) = delete;
	RenderingResources& operator=(const RenderingResources&) = delete;

	inline const RenderConfigs& GetRenderConfigs() const { return mRenderConfigs; }
	inline uint32_t GetUntouchedFrames() const { return mUntouchedFrames; }

	void Refresh();
	void UpdateCommandBuffer(
		vk::CommandBuffer& cmd,
		std::vector<RenderData*>& renderData);

private:
	vk::AttachmentDescription CreateAttachmentDescription(const erm::AttachmentData& data, vk::Format format) const;
	PipelineResources& GetOrCreatePipelineResources(RenderData& renderData);
	PipelineResources* FindPipelineResources(RenderData& renderData);

	void Reload();
	void Cleanup();

	void CreateRenderPass();
	void CreateFramebuffers();
	void CreateDescriptorPool();

	Device& mDevice;
	IWindow& mWindow;
	Renderer& mRenderer;
	const RenderConfigs mRenderConfigs;
	vk::UniqueRenderPass mRenderPass;
	std::vector<vk::UniqueFramebuffer> mFrameBuffers;
	vk::UniqueDescriptorPool mDescriptorPool;
	std::vector<PipelineResources> mPipelineResources;
	uint32_t mUntouchedFrames;
};

} // namespace erm
