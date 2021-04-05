#include "erm/rendering/data_structs/RenderingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/RenderConfigs.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/shaders/IShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

namespace erm {

RenderingResources::RenderingResources(
	Device& device,
	Renderer& renderer,
	const std::vector<SubpassData>& subpassData)
	: mDevice(device)
	, mRenderer(renderer)
	, mSubpassData(subpassData)
{
	Reload();
}

RenderingResources::RenderingResources(RenderingResources&& other)
	: mDevice(other.mDevice)
	, mRenderer(other.mRenderer)
	, mSubpassData(std::move(other.mSubpassData))
	, mRenderPass(std::move(other.mRenderPass))
	, mSwapChainFramebuffers(std::move(other.mSwapChainFramebuffers))
	, mDescriptorPool(std::move(other.mDescriptorPool))
	, mCommandBuffers(std::move(other.mCommandBuffers))
{}

vk::CommandBuffer RenderingResources::UpdateCommandBuffer(std::vector<RenderData*>& renderData, uint32_t imageIndex)
{
	vk::CommandBuffer& cmd = mCommandBuffers[imageIndex].get();
	cmd.reset({});

	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = {};
	beginInfo.pInheritanceInfo = nullptr;

	cmd.begin(beginInfo);

	for (size_t i = 0; i < renderData.size(); ++i)
	{
		PipelineResources& resources = GetOrCreatePipelineResources(renderData[i]->mRenderConfigs);
		resources.UpdateResources(cmd, *renderData[i], imageIndex);
	}

	std::array<vk::ClearValue, 2> clearValues {};
	clearValues[0].color.float32[0] = 0.0f;
	clearValues[0].color.float32[1] = 0.0f;
	clearValues[0].color.float32[2] = 0.0f;
	clearValues[0].color.float32[3] = 0.0f;
	clearValues[1].setDepthStencil({1.0f, 0});

	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = mRenderPass.get();
	renderPassInfo.framebuffer = mSwapChainFramebuffers[imageIndex].get();
	renderPassInfo.renderArea.offset = vk::Offset2D {0, 0};
	renderPassInfo.renderArea.extent = mRenderer.GetSwapChainExtent();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	cmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

	for (size_t i = 0; i < renderData.size(); ++i)
	{
		PipelineResources& resources = GetOrCreatePipelineResources(renderData[i]->mRenderConfigs);
		resources.UpdateCommandBuffer(cmd, *renderData[i], imageIndex);
	}

	cmd.endRenderPass();
	cmd.end();

	PostDraw();

	return cmd;
}

void RenderingResources::PostDraw()
{
	for (auto& res : mPipelineResources)
		res->PostDraw();
}

void RenderingResources::AddSubpass(const SubpassData& data)
{
	for (const SubpassData& subpass : mSubpassData)
	{
		if (subpass == data)
			return;
	}

	mSubpassData.emplace_back(data);
	Reload();
}

bool RenderingResources::IsSubpassCompatible(const SubpassData& subpass) const
{
	for (const SubpassData& data : mSubpassData)
	{
		if (data == subpass)
			return true;
	}
	return false;
}

void RenderingResources::Refresh()
{
	std::vector<RenderConfigs> configsToRecreate;
	for (int i = 0; i < static_cast<int>(mPipelineResources.size()); ++i)
	{
		const PipelineResources& res = *mPipelineResources[i];
		if (const RenderConfigs& configs = res.GetRenderConfigs(); configs.mShaderProgram->NeedsReload())
		{
			if (configsToRecreate.empty())
				mDevice->waitIdle();

			configsToRecreate.emplace_back(configs);
			mPipelineResources.erase(mPipelineResources.begin() + i);
			--i;
		}
	}

	for (const RenderConfigs& config : configsToRecreate)
	{
		mPipelineResources.emplace_back(std::make_unique<PipelineResources>(mDevice, mRenderer, &mRenderPass.get(), &mDescriptorPool.get(), config));
	}

	for (const RenderConfigs& config : configsToRecreate)
	{
		config.mShaderProgram->OnReloaded();
	}
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

PipelineResources& RenderingResources::GetOrCreatePipelineResources(const RenderConfigs& renderConfigs)
{
	for (const std::unique_ptr<PipelineResources>& resources : mPipelineResources)
	{
		if (renderConfigs.IsPipelineLevelCompatible(resources->GetRenderConfigs()))
		{
			return *resources;
		}
	}

	return *mPipelineResources.emplace_back(std::make_unique<PipelineResources>(mDevice, mRenderer, &mRenderPass.get(), &mDescriptorPool.get(), renderConfigs));
}

void RenderingResources::Reload()
{
	Cleanup();
	CreateRenderPass();
	CreateFramebuffers();
	CreateDescriptorPool();
	CreateCommandBuffers();
}

void RenderingResources::Cleanup()
{
	mSwapChainFramebuffers.clear();
	mCommandBuffers.clear();
	mPipelineResources.clear();
	mRenderPass.reset();
	mDescriptorPool.reset();
}

void RenderingResources::CreateRenderPass()
{
	std::deque<vk::AttachmentReference> attachmentRefs;
	std::vector<vk::AttachmentDescription> attachments;
	std::vector<vk::SubpassDescription> subpasses(mSubpassData.size());
	std::vector<vk::SubpassDependency> dependencies(mSubpassData.size());

	for (size_t i = 0; i < mSubpassData.size(); ++i)
	{
		const SubpassData& data = mSubpassData[i];

		subpasses[i].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpasses[i].colorAttachmentCount = 1;

		attachments.emplace_back(CreateAttachmentDescription(data.mColorAttachment, mRenderer.GetSwapChainImageFormat()));

		vk::AttachmentReference& colorAttachmentRef = attachmentRefs.emplace_back();
		colorAttachmentRef.attachment = 0;
#ifdef ERM_RAY_TRACING_ENABLED
		colorAttachmentRef.layout = vk::ImageLayout::eGeneral;
#else
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
#endif

		subpasses[i].pColorAttachments = &colorAttachmentRef;

		if (data.mDepthAttachment.has_value())
		{
			attachments.emplace_back(CreateAttachmentDescription(data.mDepthAttachment.value(), VkUtils::FindDepthFormat(mDevice.GetVkPhysicalDevice())));

			vk::AttachmentReference& depthAttachmentRef = attachmentRefs.emplace_back();
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

			subpasses[i].pDepthStencilAttachment = &depthAttachmentRef;
		}

		vk::SubpassDependency dependency = {};
		dependency.srcSubpass = static_cast<uint32_t>((i == 0) ? VK_SUBPASS_EXTERNAL : i - 1);
		dependency.dstSubpass = static_cast<uint32_t>((i == (mSubpassData.size() - 1) && mSubpassData.size() > 1) ? VK_SUBPASS_EXTERNAL : i);
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.srcAccessMask = {};
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

		dependencies[i] = dependency;
	}

	vk::RenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();

	mRenderPass = mDevice->createRenderPassUnique(renderPassInfo);
}

void RenderingResources::CreateFramebuffers()
{
	const std::vector<vk::ImageView>& swapChainImageViews = mRenderer.GetSwapChainImageViews();
	vk::Extent2D swapChainExtent = mRenderer.GetSwapChainExtent();

	mSwapChainFramebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		std::vector<vk::ImageView> attachments = {swapChainImageViews[i], mRenderer.GetDepthImageView()};

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = mRenderPass.get();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = static_cast<uint32_t>(swapChainExtent.width);
		framebufferInfo.height = static_cast<uint32_t>(swapChainExtent.height);
		framebufferInfo.layers = 1;

		mSwapChainFramebuffers[i] = mDevice->createFramebufferUnique(framebufferInfo);
	}
}

void RenderingResources::CreateDescriptorPool()
{
	const std::vector<vk::ImageView>& swapChainImageViews = mRenderer.GetSwapChainImageViews();

	std::array<vk::DescriptorPoolSize, 2> poolSizes {};
	poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImageViews.size() * 1000);
	poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImageViews.size() * 100);

	vk::DescriptorPoolCreateInfo poolInfo {};
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(swapChainImageViews.size() * 100);
	poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

	mDescriptorPool = mDevice->createDescriptorPoolUnique(poolInfo);
}

void RenderingResources::CreateCommandBuffers()
{
	const std::vector<vk::ImageView>& swapChainImageViews = mRenderer.GetSwapChainImageViews();

	mCommandBuffers.resize(swapChainImageViews.size());

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = mDevice.GetCommandPool();
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

	mCommandBuffers = mDevice->allocateCommandBuffersUnique(allocInfo);
}

} // namespace erm
