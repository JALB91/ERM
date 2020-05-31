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
		const RenderConfigs& renderConfigs)
		: mDevice(device)
		, mRenderer(renderer)
		, mRenderConfigs(renderConfigs)
	{
		CreateRenderPass();
		CreatePipeline();
		CreateFramebuffers();
		CreateDescriptorPool();
		CreateCommandBuffers();
	}

	RenderingResources::~RenderingResources()
	{
		mDevice->destroyDescriptorSetLayout(mDescriptorSetLayout);
		for (size_t i = 0; i < mRenderer.GetSwapChainImageViews().size(); ++i)
		{
			mDevice->destroyFramebuffer(mSwapChainFramebuffers[i]);
		}
		mBindingResources.clear();
		mDevice->freeCommandBuffers(mDevice.GetCommandPool(), mCommandBuffers.size(), mCommandBuffers.data());
		mDevice->destroyPipeline(mPipeline);
		mDevice->destroyPipelineLayout(mPipelineLayout);
		mDevice->destroyRenderPass(mRenderPass);
		mDevice->destroyDescriptorPool(mDescriptorPool);
	}

	void RenderingResources::CreateDescriptorPool(vk::DescriptorPoolCreateInfo& info)
	{
		if (mDescriptorPool)
		{
			mBindingResources.clear();
			mDevice->destroyDescriptorPool(mDescriptorPool);
		}

		mDescriptorPool = mDevice->createDescriptorPool(info);
	}

	void RenderingResources::UpdateRenderingResources(std::vector<RenderData*>& renderData, uint32_t imageIndex)
	{
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
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);

		std::sort(renderData.begin(), renderData.end(), [](RenderData* a, RenderData* b) {
			const bool aHasId = a->mRenderingId.has_value();
			const bool bHasId = b->mRenderingId.has_value();
			if (aHasId && !bHasId)
				return true;
			else if (!aHasId && bHasId)
				return false;
			else if (aHasId && bHasId)
				return a->mRenderingId.value() <= b->mRenderingId.value();
			return true;
		});

		for (size_t i = 0; i < renderData.size(); ++i)
		{
			RenderData* data = renderData[i];

			if (!data->mRenderingId.has_value() || data->mRenderingId.value() >= mBindingResources.size())
			{
				data->mRenderingId = mBindingResources.size();
				mBindingResources.emplace_back(*this, data->mRenderConfigs);
			}

			BindingResources& resources = mBindingResources[data->mRenderingId.value()];
			resources.UpdateResources(*data, imageIndex);

			for (const Mesh* mesh : data->mMehses)
			{
				cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, 1, resources.GetDescriptorSet(imageIndex), 0, nullptr);
				mesh->GetVertexBuffer().Bind(cmd);
				mesh->GetIndexBuffer().Bind(cmd);
				cmd.drawIndexed(mesh->GetIndexBuffer().GetCount(), 1, 0, 0, 0);
			}
		}

		cmd.endRenderPass();
		cmd.end();
	}

	void RenderingResources::CreateRenderPass()
	{
		vk::AttachmentDescription colorAttachment = {};
		colorAttachment.format = mRenderer.GetSwapChainImageFormat();
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::AttachmentDescription depthAttachment {};
		depthAttachment.format = VkUtils::FindDepthFormat(mDevice.GetVkPhysicalDevice());
		depthAttachment.samples = vk::SampleCountFlagBits::e1;
		depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
		depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentReference depthAttachmentRef {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		vk::SubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.srcAccessMask = {};
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

		std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
		vk::RenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		mRenderPass = mDevice->createRenderPass(renderPassInfo);
	}

	void RenderingResources::CreatePipeline()
	{
		const vk::Extent2D extent = mRenderer.GetSwapChainExtent();

		/*
			LOAD SHADERS
		*/
		vk::ShaderModule vertShaderModule = mRenderConfigs.mShaderProgram->CreateVertexShaderModule();
		vk::ShaderModule fragShaderModule = mRenderConfigs.mShaderProgram->CreateFragmentShaderModule();

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		/*
			SETUP VERTEX INPUT
		*/
		vk::VertexInputBindingDescription bindingDescription = VkUtils::GetVertexBindingDescription();
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = VkUtils::GetVertexAttributeDescriptions();

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		/*
			SETUP INPUT ASSEMBLY
		*/
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.topology = VkUtils::ToVulkanValue<vk::PrimitiveTopology>(mRenderConfigs.mDrawMode);
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		/*
			SETUP VIEWPORT AND SCISSOR
		*/
		vk::Viewport viewport = {};
		viewport.x = mRenderConfigs.mViewport.mMin.x * extent.width;
		viewport.y = mRenderConfigs.mViewport.mMin.y * extent.height;
		viewport.width = mRenderConfigs.mViewport.mMax.x * extent.width;
		viewport.height = mRenderConfigs.mViewport.mMax.y * extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor = {};
		scissor.offset = vk::Offset2D(static_cast<uint32_t>(viewport.x), static_cast<uint32_t>(viewport.y));
		scissor.extent = vk::Extent2D(static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height));

		vk::PipelineViewportStateCreateInfo viewportState = {};
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		/*
			SETUP RASTERIZER
		*/
		vk::PipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VkUtils::ToVulkanValue<vk::PolygonMode>(mRenderConfigs.mPolygonMode);
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VkUtils::ToVulkanValue<vk::CullModeFlagBits>(mRenderConfigs.mCullMode);
		rasterizer.frontFace = VkUtils::ToVulkanValue<vk::FrontFace>(mRenderConfigs.mFrontFace);
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		/*
			SETUP MULTISAMPLING
		*/
		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		/*
			SETUP COLOR BLENDING
		*/
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
		colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
		colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
		colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

		vk::PipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		/*
			SETUP DESCRIPTOR SET LAYOUT
		*/
		vk::DescriptorSetLayoutBinding uboLayoutBinding {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
		uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;

		vk::DescriptorSetLayoutBinding samplerLayoutBinding {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
		samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;

		std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

		vk::DescriptorSetLayoutCreateInfo layoutInfo {};
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		mDescriptorSetLayout = mDevice->createDescriptorSetLayout(layoutInfo);

		/*
			SETUP PIPELINE LAYOUT
		*/
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		mPipelineLayout = mDevice->createPipelineLayout(pipelineLayoutInfo);

		/*
			SETUP STENCIL AND DEPTH TESTS
		*/
		vk::PipelineDepthStencilStateCreateInfo depthStencil {};
		depthStencil.depthTestEnable = mRenderConfigs.mDepthTestEnabled ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = mRenderConfigs.mDepthWriteEnabled ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = VkUtils::ToVulkanValue<vk::CompareOp>(mRenderConfigs.mDepthFunction);
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;

		/*
			CREATE PIPELINE
		*/
		vk::GraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;
		pipelineInfo.layout = mPipelineLayout;
		pipelineInfo.renderPass = mRenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = nullptr;
		pipelineInfo.basePipelineIndex = -1;

		mPipeline = mDevice->createGraphicsPipeline(nullptr, pipelineInfo, nullptr);

		mDevice->destroyShaderModule(vertShaderModule);
		mDevice->destroyShaderModule(fragShaderModule);
	}

	void RenderingResources::CreateFramebuffers()
	{
		const std::vector<vk::ImageView>& swapChainImageViews = mRenderer.GetSwapChainImageViews();
		const math::vec2 viewportSize = mRenderConfigs.mViewport.GetSize();
		vk::Extent2D swapChainExtent = mRenderer.GetSwapChainExtent();

		mSwapChainFramebuffers.resize(swapChainImageViews.size());
		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			std::vector<vk::ImageView> attachments = {swapChainImageViews[i], mRenderer.GetDepthImageView()};

			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.renderPass = mRenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = static_cast<uint32_t>(viewportSize.x * swapChainExtent.width);
			framebufferInfo.height = static_cast<uint32_t>(viewportSize.y * swapChainExtent.height);
			framebufferInfo.layers = 1;

			mSwapChainFramebuffers[i] = mDevice->createFramebuffer(framebufferInfo);
		}
	}

	void RenderingResources::CreateDescriptorPool()
	{
		const std::vector<vk::ImageView>& swapChainImageViews = mRenderer.GetSwapChainImageViews();

		std::array<vk::DescriptorPoolSize, 2> poolSizes {};
		poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImageViews.size() * 100);
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
