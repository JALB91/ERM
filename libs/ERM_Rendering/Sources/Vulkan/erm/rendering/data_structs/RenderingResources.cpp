#include "erm/rendering/data_structs/RenderingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/AttachmentData.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/textures/GPUImage.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/utils/Utils.h>

#include <deque>

namespace erm {

RenderingResources::RenderingResources(
	Device& device,
	IWindow& window,
	Renderer& renderer,
	const RenderConfigs& renderConfigs
)
	: mDevice(device)
	, mWindow(window)
	, mRenderer(renderer)
	, mRenderConfigs(renderConfigs)
	, mUntouchedFrames(0)
{
	reload();
}

void RenderingResources::refresh()
{
	for (auto it = mPipelineResources.begin(); it != mPipelineResources.end();)
	{
		auto& resources = *it;
		
		if (resources.getUntouchedFrames() > IRenderer::kMaxFramesInFlight)
		{
			it = mPipelineResources.erase(it);
		}
		else
		{
			resources.refresh();
			++it;
		}
	}
	
	++mUntouchedFrames;
}

void RenderingResources::updateCommandBuffer(
	vk::CommandBuffer& cmd,
	std::vector<RenderData*>& renderData)
{
	mUntouchedFrames = 0;
	
	for (auto* data : renderData)
	{
		auto& resources = getOrcreatePipelineResources(*data);
		resources.updateResources(cmd, *data);
	}

	std::vector<vk::ClearValue> clearValues {1};
	clearValues[0].color.float32[0] = mRenderConfigs.mClearColor.r;
	clearValues[0].color.float32[1] = mRenderConfigs.mClearColor.g;
	clearValues[0].color.float32[2] = mRenderConfigs.mClearColor.b;
	clearValues[0].color.float32[3] = mRenderConfigs.mClearColor.a;

	if (mRenderConfigs.mSubpassData.mDepthAttachment.has_value())
	{
		clearValues.emplace_back().setDepthStencil({1.0f, 0});
	}

	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = mRenderPass.get();
	renderPassInfo.framebuffer = mFrameBuffers[std::min(mFrameBuffers.size() - 1, static_cast<u64>(mRenderer.getCurrentImageIndex()))].get();
	renderPassInfo.renderArea.offset = vk::Offset2D {0, 0};
	renderPassInfo.renderArea.extent = mRenderer.getSwapChainExtent();
	renderPassInfo.clearValueCount = static_cast<u32>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	cmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	for (auto* data : renderData)
	{
		auto* resources = findPipelineResources(*data);
		ERM_ASSERT(resources != nullptr);
		resources->updateCommandBuffer(cmd, *data);
	}

	cmd.endRenderPass();
}

vk::AttachmentDescription RenderingResources::createAttachmentDescription(const erm::AttachmentData& data, vk::Format format) const
{
	vk::AttachmentDescription attachment = {};
	attachment.format = format;
	attachment.samples = vk::SampleCountFlagBits::e1;
	attachment.loadOp = VkUtils::toVulkanValue<vk::AttachmentLoadOp>(data.mLoadOp);
	attachment.storeOp = VkUtils::toVulkanValue<vk::AttachmentStoreOp>(data.mStoreOp);
	attachment.stencilLoadOp = VkUtils::toVulkanValue<vk::AttachmentLoadOp>(data.getStencilLoadOp());
	attachment.stencilStoreOp = VkUtils::toVulkanValue<vk::AttachmentStoreOp>(data.getStencilStoreOp());
	attachment.initialLayout = VkUtils::toVulkanValue<vk::ImageLayout>(data.mInitialLayout);
	attachment.finalLayout = VkUtils::toVulkanValue<vk::ImageLayout>(data.mFinalLayout);
	return attachment;
}

PipelineResources& RenderingResources::getOrcreatePipelineResources(RenderData& renderData)
{
	if (auto* resources = findPipelineResources(renderData))
	{
		return *resources;
	}

	return mPipelineResources.emplace_back(
		mDevice,
		mWindow,
		mRenderer,
		&mRenderPass.get(),
		&mDescriptorPool.get(),
		renderData.mPipelineConfigs);
}

PipelineResources* RenderingResources::findPipelineResources(RenderData& renderData)
{
	for (auto& resources : mPipelineResources)
	{
		if (renderData.mPipelineConfigs.isPipelineLevelCompatible(resources.getPipelineConfigs()))
		{
			return &resources;
		}
	}
	
	return nullptr;
}

void RenderingResources::reload()
{
	cleanup();
	createRenderPass();
	createFramebuffers();
	createDescriptorPool();
}

void RenderingResources::cleanup()
{
	mFrameBuffers.clear();
	mPipelineResources.clear();
	mRenderPass.reset();
	mDescriptorPool.reset();
}

void RenderingResources::createRenderPass()
{
	std::deque<vk::AttachmentReference> attachmentRefs;
	std::vector<vk::AttachmentDescription> attachments;
	vk::SubpassDescription subpass;
	vk::SubpassDependency dependency;

	const auto& data = mRenderConfigs.mSubpassData;

	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;

	const auto& frameBuffers = mRenderer.getTargetFrameBuffers(mRenderConfigs.mSubpassData.mColorAttachment.mFrameBufferType);
	ERM_ASSERT(!frameBuffers.empty());

	attachments.emplace_back(createAttachmentDescription(data.mColorAttachment, frameBuffers[0]->getImageFormat()));

	auto& colorAttachmentRef = attachmentRefs.emplace_back();
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VkUtils::toVulkanValue<vk::ImageLayout>(data.mColorAttachment.mFinalLayout);

	subpass.pColorAttachments = &colorAttachmentRef;

	if (data.mDepthAttachment.has_value())
	{
		const std::vector<GPUTexture*>& depthFrameBuffers = mRenderer.getTargetFrameBuffers(data.mDepthAttachment.value().mFrameBufferType);
		ERM_ASSERT(!depthFrameBuffers.empty());

		attachments.emplace_back(createAttachmentDescription(data.mDepthAttachment.value(), depthFrameBuffers[0]->getImageFormat()));

		vk::AttachmentReference& depthAttachmentRef = attachmentRefs.emplace_back();
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VkUtils::toVulkanValue<vk::ImageLayout>(data.mDepthAttachment.value().mFinalLayout);

		subpass.pDepthStencilAttachment = &depthAttachmentRef;
	}

	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.srcAccessMask = {};
	dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
	dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

	vk::RenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.attachmentCount = static_cast<u32>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	ERM_VK_CHECK_AND_ASSIGN(mRenderPass, mDevice->createRenderPassUnique(renderPassInfo));
}

void RenderingResources::createFramebuffers()
{
	const auto& frameBuffers = mRenderer.getTargetFrameBuffers(mRenderConfigs.mSubpassData.mColorAttachment.mFrameBufferType);
	const auto& swapChainExtent = mRenderer.getSwapChainExtent();

	mFrameBuffers.resize(frameBuffers.size());
	
	for (u64 i = 0; i < frameBuffers.size(); i++)
	{
		std::vector<vk::ImageView> attachments = {frameBuffers[i]->getImageView()};

		if (mRenderConfigs.mSubpassData.mDepthAttachment.has_value())
		{
			attachments.emplace_back(mRenderer.getTargetFrameBuffers(mRenderConfigs.mSubpassData.mDepthAttachment.value().mFrameBufferType)[0]->getImageView());
		}

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = mRenderPass.get();
		framebufferInfo.attachmentCount = static_cast<u32>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = static_cast<u32>(swapChainExtent.width);
		framebufferInfo.height = static_cast<u32>(swapChainExtent.height);
		framebufferInfo.layers = 1;

		ERM_VK_CHECK_AND_ASSIGN(mFrameBuffers[i], mDevice->createFramebufferUnique(framebufferInfo));
	}
}

void RenderingResources::createDescriptorPool()
{
	const auto& frameBuffers = mRenderer.getTargetFrameBuffers(mRenderConfigs.mSubpassData.mColorAttachment.mFrameBufferType);

	std::array<vk::DescriptorPoolSize, 3> poolSizes {};
	poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
	poolSizes[0].descriptorCount = static_cast<u32>(frameBuffers.size() * 1000);
	poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
	poolSizes[1].descriptorCount = static_cast<u32>(frameBuffers.size() * 100);
	poolSizes[2].type = vk::DescriptorType::eStorageImage;
	poolSizes[2].descriptorCount = static_cast<u32>(frameBuffers.size() * 100);

	vk::DescriptorPoolCreateInfo poolInfo {};
	poolInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<u32>(frameBuffers.size() * 10000);
	poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

	ERM_VK_CHECK_AND_ASSIGN(mDescriptorPool, mDevice->createDescriptorPoolUnique(poolInfo));
}

} // namespace erm
