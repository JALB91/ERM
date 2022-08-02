#include "erm/rendering/data_structs/RenderingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/AttachmentData.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/textures/Texture.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

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
{
	Reload();
}

void RenderingResources::Refresh()
{
	for (auto& resources : mPipelineResources)
		resources->Refresh();
}

void RenderingResources::UpdateCommandBuffer(
	vk::CommandBuffer& cmd,
	std::vector<RenderData*>& renderData)
{
	for (RenderData* data : renderData)
	{
		PipelineResources& resources = GetOrCreatePipelineResources(data->mPipelineConfigs);
		resources.UpdateResources(cmd, *data);
	}

	std::vector<vk::ClearValue> clearValues {1};
	clearValues[0].color.float32[0] = 0.0f;
	clearValues[0].color.float32[1] = 0.0f;
	clearValues[0].color.float32[2] = 0.0f;
	clearValues[0].color.float32[3] = 0.0f;

	if (mRenderConfigs.mSubpassData.mDepthAttachment.has_value())
		clearValues.emplace_back().setDepthStencil({1.0f, 0});

	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = mRenderPass.get();
	renderPassInfo.framebuffer = mFrameBuffers[std::min(mFrameBuffers.size() - 1, static_cast<size_t>(mRenderer.GetCurrentImageIndex()))].get();
	renderPassInfo.renderArea.offset = vk::Offset2D {0, 0};
	renderPassInfo.renderArea.extent = mRenderer.GetSwapChainExtent();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	cmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	for (RenderData* data : renderData)
	{
		PipelineResources& resources = GetOrCreatePipelineResources(data->mPipelineConfigs);
		resources.UpdateCommandBuffer(cmd, *data);
	}

	cmd.endRenderPass();
}

vk::AttachmentDescription RenderingResources::CreateAttachmentDescription(const erm::AttachmentData& data, vk::Format format) const
{
	vk::AttachmentDescription attachment = {};
	attachment.format = format;
	attachment.samples = vk::SampleCountFlagBits::e1;
	attachment.loadOp = VkUtils::ToVulkanValue<vk::AttachmentLoadOp>(data.mLoadOp);
	attachment.storeOp = VkUtils::ToVulkanValue<vk::AttachmentStoreOp>(data.mStoreOp);
	attachment.stencilLoadOp = VkUtils::ToVulkanValue<vk::AttachmentLoadOp>(data.GetStencilLoadOp());
	attachment.stencilStoreOp = VkUtils::ToVulkanValue<vk::AttachmentStoreOp>(data.GetStencilStoreOp());
	attachment.initialLayout = VkUtils::ToVulkanValue<vk::ImageLayout>(data.mInitialLayout);
	attachment.finalLayout = VkUtils::ToVulkanValue<vk::ImageLayout>(data.mFinalLayout);
	return attachment;
}

PipelineResources& RenderingResources::GetOrCreatePipelineResources(const PipelineConfigs& pipelineConfigs)
{
	for (const std::unique_ptr<PipelineResources>& resources : mPipelineResources)
	{
		if (pipelineConfigs.IsPipelineLevelCompatible(resources->GetPipelineConfigs()))
		{
			return *resources;
		}
	}

	return *mPipelineResources.emplace_back(std::make_unique<PipelineResources>(mDevice, mWindow, mRenderer, &mRenderPass.get(), &mDescriptorPool.get(), pipelineConfigs));
}

void RenderingResources::Reload()
{
	Cleanup();
	CreateRenderPass();
	CreateFramebuffers();
	CreateDescriptorPool();
}

void RenderingResources::Cleanup()
{
	mFrameBuffers.clear();
	mPipelineResources.clear();
	mRenderPass.reset();
	mDescriptorPool.reset();
}

void RenderingResources::CreateRenderPass()
{
	std::deque<vk::AttachmentReference> attachmentRefs;
	std::vector<vk::AttachmentDescription> attachments;
	vk::SubpassDescription subpass;
	vk::SubpassDependency dependency;

	const SubpassData& data = mRenderConfigs.mSubpassData;

	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;

	const std::vector<Texture*>& frameBuffers = mRenderer.GetTargetFrameBuffers(mRenderConfigs.mSubpassData.mColorAttachment.mFrameBufferType);
	ERM_ASSERT(!frameBuffers.empty());

	attachments.emplace_back(CreateAttachmentDescription(data.mColorAttachment, frameBuffers[0]->GetImageFormat()));

	vk::AttachmentReference& colorAttachmentRef = attachmentRefs.emplace_back();
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VkUtils::ToVulkanValue<vk::ImageLayout>(data.mColorAttachment.mFinalLayout);

	subpass.pColorAttachments = &colorAttachmentRef;

	if (data.mDepthAttachment.has_value())
	{
		const std::vector<Texture*>& depthFrameBuffers = mRenderer.GetTargetFrameBuffers(data.mDepthAttachment.value().mFrameBufferType);
		ERM_ASSERT(!depthFrameBuffers.empty());

		attachments.emplace_back(CreateAttachmentDescription(data.mDepthAttachment.value(), depthFrameBuffers[0]->GetImageFormat()));

		vk::AttachmentReference& depthAttachmentRef = attachmentRefs.emplace_back();
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VkUtils::ToVulkanValue<vk::ImageLayout>(data.mDepthAttachment.value().mFinalLayout);

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
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	ERM_VK_CHECK_AND_ASSIGN(mRenderPass, mDevice->createRenderPassUnique(renderPassInfo));
}

void RenderingResources::CreateFramebuffers()
{
	const std::vector<Texture*>& frameBuffers = mRenderer.GetTargetFrameBuffers(mRenderConfigs.mSubpassData.mColorAttachment.mFrameBufferType);
	const vk::Extent2D& swapChainExtent = mRenderer.GetSwapChainExtent();

	mFrameBuffers.resize(frameBuffers.size());
	for (size_t i = 0; i < frameBuffers.size(); i++)
	{
		std::vector<vk::ImageView> attachments = {frameBuffers[i]->GetImageView()};

		if (mRenderConfigs.mSubpassData.mDepthAttachment.has_value())
		{
			attachments.emplace_back(mRenderer.GetTargetFrameBuffers(mRenderConfigs.mSubpassData.mDepthAttachment.value().mFrameBufferType)[0]->GetImageView());
		}

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = mRenderPass.get();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = static_cast<uint32_t>(swapChainExtent.width);
		framebufferInfo.height = static_cast<uint32_t>(swapChainExtent.height);
		framebufferInfo.layers = 1;

		ERM_VK_CHECK_AND_ASSIGN(mFrameBuffers[i], mDevice->createFramebufferUnique(framebufferInfo));
	}
}

void RenderingResources::CreateDescriptorPool()
{
	const std::vector<Texture*>& frameBuffers = mRenderer.GetTargetFrameBuffers(mRenderConfigs.mSubpassData.mColorAttachment.mFrameBufferType);

	std::array<vk::DescriptorPoolSize, 2> poolSizes {};
	poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(frameBuffers.size() * 1000);
	poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(frameBuffers.size() * 100);

	vk::DescriptorPoolCreateInfo poolInfo {};
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(frameBuffers.size() * 100);
	poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

	ERM_VK_CHECK_AND_ASSIGN(mDescriptorPool, mDevice->createDescriptorPoolUnique(poolInfo));
}

} // namespace erm
