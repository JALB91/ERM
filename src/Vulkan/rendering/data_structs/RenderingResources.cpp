#include "erm/rendering/data_structs/RenderingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/data_structs/Mesh.h"
#include "erm/rendering/data_structs/RenderConfigs.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/shaders/ShaderProgram.h"
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
		, mRenderPass(nullptr)
		, mDescriptorPool(nullptr)
	{
		Reload();
	}

	RenderingResources::~RenderingResources()
	{
		Cleanup();
	}

	RenderingResources::RenderingResources(RenderingResources&& other)
		: mDevice(other.mDevice)
		, mRenderer(other.mRenderer)
		, mSubpassData(other.mSubpassData)
		, mRenderPass(other.mRenderPass)
		, mSwapChainFramebuffers(std::move(other.mSwapChainFramebuffers))
		, mDescriptorPool(other.mDescriptorPool)
		, mCommandBuffers(std::move(other.mCommandBuffers))
	{
		other.mRenderPass = nullptr;
		other.mDescriptorPool = nullptr;
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

	void RenderingResources::Update(std::vector<RenderData*>& renderData, uint32_t imageIndex)
	{
		std::sort(renderData.begin(), renderData.end(), [](RenderData* a, RenderData* b) -> bool {
			const bool aHasId = a->mRenderingId.has_value();
			const bool bHasId = b->mRenderingId.has_value();
			if (aHasId && !bHasId)
				return true;
			else if (!aHasId && bHasId)
				return false;
			else if (aHasId && bHasId)
				return a->mRenderingId.value() < b->mRenderingId.value();
			return false;
		});

		vk::CommandBuffer& cmd = mCommandBuffers[imageIndex];
		cmd.reset({});

		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = {};
		beginInfo.pInheritanceInfo = nullptr;

		cmd.begin(beginInfo);

		std::array<vk::ClearValue, 2> clearValues {};
		clearValues[0].color.float32[0] = 0.0f;
		clearValues[0].color.float32[1] = 0.0f;
		clearValues[0].color.float32[2] = 0.0f;
		clearValues[0].color.float32[3] = 0.0f;
		clearValues[1].setDepthStencil({1.0f, 0});

		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass = mRenderPass;
		renderPassInfo.framebuffer = mSwapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = vk::Offset2D {0, 0};
		renderPassInfo.renderArea.extent = mRenderer.GetSwapChainExtent();
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		cmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

		for (size_t i = 0; i < renderData.size(); ++i)
		{
			PipelineResources& resources = GetOrCreatePipelineResources(renderData[i]->mRenderConfigs);
			resources.Update(cmd, *renderData[i], imageIndex);
		}

		cmd.endRenderPass();
		cmd.end();
	}

	void RenderingResources::Refresh()
	{
		std::vector<RenderConfigs> configsToRecreate;
		for (int i = 0; i < static_cast<int>(mPipelineResources.size()); ++i)
		{
			const PipelineResources& res = *mPipelineResources[i];
			if (res.mRenderConfigs.mShaderProgram->NeedsReload())
			{
				if (configsToRecreate.empty())
					mDevice->waitIdle();

				configsToRecreate.emplace_back(res.mRenderConfigs);
				mPipelineResources.erase(mPipelineResources.begin() + i);
				--i;
			}
		}

		for (const RenderConfigs& config : configsToRecreate)
		{
			mPipelineResources.emplace_back(std::make_unique<PipelineResources>(mDevice, mRenderer, &mRenderPass, &mDescriptorPool, config));
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
			if (resources->mRenderConfigs.IsPipelineLevelCompatible(renderConfigs))
			{
				return *resources;
			}
		}

		return *mPipelineResources.emplace_back(std::make_unique<PipelineResources>(mDevice, mRenderer, &mRenderPass, &mDescriptorPool, renderConfigs));
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
		for (size_t i = 0; i < mSwapChainFramebuffers.size(); ++i)
		{
			mDevice->destroyFramebuffer(mSwapChainFramebuffers[i]);
		}
		mSwapChainFramebuffers.clear();
		mDevice->freeCommandBuffers(mDevice.GetCommandPool(), static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());
		mCommandBuffers.clear();
		mPipelineResources.clear();
		mDevice->destroyRenderPass(mRenderPass);
		mRenderPass = nullptr;
		mDevice->destroyDescriptorPool(mDescriptorPool);
		mDescriptorPool = nullptr;
	}

	void RenderingResources::CreateRenderPass()
	{
		std::vector<vk::AttachmentDescription> attachments;
		std::vector<vk::SubpassDescription> subpasses(mSubpassData.size());
		std::vector<vk::SubpassDependency> dependencies(mSubpassData.size());

		for (size_t i = 0; i < mSubpassData.size(); ++i)
		{
			const SubpassData& data = mSubpassData[i];

			subpasses[i].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpasses[i].colorAttachmentCount = 1;

			attachments.emplace_back(CreateAttachmentDescription(data.mColorAttachment, mRenderer.GetSwapChainImageFormat()));

			vk::AttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

			subpasses[i].pColorAttachments = &colorAttachmentRef;

			if (data.mDepthAttachment.has_value())
			{
				attachments.emplace_back(CreateAttachmentDescription(data.mDepthAttachment.value(), VkUtils::FindDepthFormat(mDevice.GetVkPhysicalDevice())));

				vk::AttachmentReference depthAttachmentRef {};
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

		mRenderPass = mDevice->createRenderPass(renderPassInfo);
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
			framebufferInfo.renderPass = mRenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = static_cast<uint32_t>(swapChainExtent.width);
			framebufferInfo.height = static_cast<uint32_t>(swapChainExtent.height);
			framebufferInfo.layers = 1;

			mSwapChainFramebuffers[i] = mDevice->createFramebuffer(framebufferInfo);
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

		mDescriptorPool = mDevice->createDescriptorPool(poolInfo);
	}

	void RenderingResources::CreateCommandBuffers()
	{
		const std::vector<vk::ImageView>& swapChainImageViews = mRenderer.GetSwapChainImageViews();

		mCommandBuffers.resize(swapChainImageViews.size());

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = mDevice.GetCommandPool();
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

		mCommandBuffers = mDevice->allocateCommandBuffers(allocInfo);
	}

} // namespace erm
