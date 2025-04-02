#pragma once

#include "erm/rendering/data_structs/PipelineResources.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <erm/math/Types.h>

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

	inline const RenderConfigs& getRenderConfigs() const { return mRenderConfigs; }
	inline u32 getUntouchedFrames() const { return mUntouchedFrames; }

	void refresh();
	void updateCommandBuffer(
		vk::CommandBuffer& cmd,
		std::vector<RenderData*>& renderData);

private:
	vk::AttachmentDescription createAttachmentDescription(const erm::AttachmentData& data, vk::Format format) const;
	PipelineResources& getOrcreatePipelineResources(RenderData& renderData);
	PipelineResources* findPipelineResources(RenderData& renderData);

	void reload();
	void cleanup();

	void createRenderPass();
	void createFramebuffers();
	void createDescriptorPool();

	Device& mDevice;
	IWindow& mWindow;
	Renderer& mRenderer;
	const RenderConfigs mRenderConfigs;
	vk::UniqueRenderPass mRenderPass;
	std::vector<vk::UniqueFramebuffer> mFrameBuffers;
	vk::UniqueDescriptorPool mDescriptorPool;
	std::vector<PipelineResources> mPipelineResources;
	u32 mUntouchedFrames;
};

} // namespace erm
