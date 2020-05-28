#include "erm/rendering/data_structs/RenderingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/RenderConfigs.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

namespace erm {

	RenderingResources::RenderingResources(
		Device& device,
		const RenderConfigs& renderConfigs,
		const std::vector<vk::ImageView>& swapChainImageViews,
		vk::Format swapChainImageFormat,
		vk::Sampler textureSampler)
		: mDevice(device)
		, mRenderConfigs(renderConfigs)
		, mSwapChainImageViews(swapChainImageViews)
		, mSwapChainImageFormat(swapChainImageFormat)
		, mTextureSampler(textureSampler)
	{
		CreateRenderPass();
		CreatePipeline();
		CreateDepthResources();
		CreateFramebuffers();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
	}

	RenderingResources::~RenderingResources()
	{
		mDevice->destroyDescriptorSetLayout(mDescriptorSetLayout);
		mDevice->destroyImageView(mDepthImageView);
		mDevice->destroyImage(mDepthImage);
		mDevice->freeMemory(mDepthImageMemory);
		for (size_t i = 0; i < mSwapChainImageViews.size(); ++i)
		{
			mDevice->destroyFramebuffer(mSwapChainFramebuffers[i]);
		}
		mDevice->freeCommandBuffers(mDevice.GetCommandPool(), static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());
		mDevice->destroyPipeline(mPipeline);
		mDevice->destroyPipelineLayout(mPipelineLayout);
		mDevice->destroyRenderPass(mRenderPass);
		for (size_t i = 0; i < mSwapChainImageViews.size(); ++i)
		{
			mDevice->destroyBuffer(mUniformBuffers[i]);
			mDevice->freeMemory(mUniformBuffersMemory[i]);
		}
		mDevice->destroyDescriptorPool(mDescriptorPool);
	}

	void RenderingResources::UpdateUniformBuffer(uint32_t index, const UniformBuffer& ub) const
	{
		void* data = mDevice->mapMemory(mUniformBuffersMemory[index], 0, sizeof(UniformBuffer));
		memcpy(data, &ub, sizeof(UniformBuffer));
		mDevice->unmapMemory(mUniformBuffersMemory[index]);
	}

	void RenderingResources::CreateRenderPass()
	{
		vk::AttachmentDescription colorAttachment = {};
		colorAttachment.format = mSwapChainImageFormat;
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
		const math::vec2 viewportSize = mRenderConfigs.mViewport.GetSize();

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
		viewport.x = mRenderConfigs.mViewport.mMin.x;
		viewport.y = mRenderConfigs.mViewport.mMin.y;
		viewport.width = mRenderConfigs.mViewport.mMax.x;
		viewport.height = mRenderConfigs.mViewport.mMax.y;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor = {};
		scissor.offset = vk::Offset2D(static_cast<uint32_t>(mRenderConfigs.mViewport.mMin.x), static_cast<uint32_t>(mRenderConfigs.mViewport.mMin.y));
		scissor.extent = vk::Extent2D(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));

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
		rasterizer.cullMode = vk::CullModeFlagBits::eNone;
		rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		/*
			SETUP MULTISAMPLING
		*/
		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		/*
			SETUP COLOR BLENDING
		*/
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
		colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
		colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
		colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional

		vk::PipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

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
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		mPipelineLayout = mDevice->createPipelineLayout(pipelineLayoutInfo);

		/*
			SETUP STENCIL AND DEPTH TESTS
		*/
		vk::PipelineDepthStencilStateCreateInfo depthStencil {};
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = vk::CompareOp::eLess;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
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
		pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = mPipelineLayout;
		pipelineInfo.renderPass = mRenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = nullptr; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		mPipeline = mDevice->createGraphicsPipeline(nullptr, pipelineInfo, nullptr);

		mDevice->destroyShaderModule(vertShaderModule);
		mDevice->destroyShaderModule(fragShaderModule);
	}

	void RenderingResources::CreateDepthResources()
	{
		const math::vec2 viewportSize = mRenderConfigs.mViewport.GetSize();
		vk::Format depthFormat = VkUtils::FindDepthFormat(mDevice.GetVkPhysicalDevice());
		VkUtils::CreateImage(
			mDevice.GetVkPhysicalDevice(),
			mDevice.GetVkDevice(),
			static_cast<uint32_t>(viewportSize.x),
			static_cast<uint32_t>(viewportSize.y),
			depthFormat,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eDepthStencilAttachment,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			mDepthImage,
			mDepthImageMemory);
		mDepthImageView = VkUtils::CreateImageView(mDevice.GetVkDevice(), mDepthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
	}

	void RenderingResources::CreateFramebuffers()
	{
		const math::vec2 viewportSize = mRenderConfigs.mViewport.GetSize();

		mSwapChainFramebuffers.resize(mSwapChainImageViews.size());
		for (size_t i = 0; i < mSwapChainImageViews.size(); i++)
		{
			std::vector<vk::ImageView> attachments = {mSwapChainImageViews[i], mDepthImageView};

			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.renderPass = mRenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = static_cast<uint32_t>(viewportSize.x);
			framebufferInfo.height = static_cast<uint32_t>(viewportSize.y);
			framebufferInfo.layers = 1;

			mSwapChainFramebuffers[i] = mDevice->createFramebuffer(framebufferInfo);
		}
	}

	void RenderingResources::CreateUniformBuffers()
	{
		vk::DeviceSize bufferSize = sizeof(UniformBuffer);

		mUniformBuffers.resize(mSwapChainImageViews.size());
		mUniformBuffersMemory.resize(mSwapChainImageViews.size());

		for (size_t i = 0; i < mSwapChainImageViews.size(); i++)
		{
			VkUtils::CreateBuffer(
				mDevice.GetVkPhysicalDevice(),
				mDevice.GetVkDevice(),
				bufferSize,
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				mUniformBuffers[i],
				mUniformBuffersMemory[i]);
		}
	}

	void RenderingResources::CreateDescriptorPool()
	{
		std::array<vk::DescriptorPoolSize, 2> poolSizes {};
		poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(mSwapChainImageViews.size());
		poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(mSwapChainImageViews.size());

		vk::DescriptorPoolCreateInfo poolInfo {};
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(mSwapChainImageViews.size());
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

		mDescriptorPool = mDevice->createDescriptorPool(poolInfo);
	}

	void RenderingResources::CreateDescriptorSets()
	{
		std::vector<vk::DescriptorSetLayout> layouts(mSwapChainImageViews.size(), mDescriptorSetLayout);

		vk::DescriptorSetAllocateInfo allocInfo {};
		allocInfo.descriptorPool = mDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(mSwapChainImageViews.size());
		allocInfo.pSetLayouts = layouts.data();

		mDescriptorSets.resize(mSwapChainImageViews.size());
		mDescriptorSets = mDevice->allocateDescriptorSets(allocInfo);

		for (size_t i = 0; i < mSwapChainImageViews.size(); ++i)
		{
			vk::DescriptorBufferInfo bufferInfo {};
			bufferInfo.buffer = mUniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBuffer);

			vk::DescriptorImageInfo imageInfo {};
			imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imageInfo.imageView = mRenderConfigs.mTexture->GetImageView();
			imageInfo.sampler = mTextureSampler;

			std::array<vk::WriteDescriptorSet, 2> descriptorWrites {};

			descriptorWrites[0].dstSet = mDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].dstSet = mDescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void RenderingResources::CreateCommandBuffers()
	{
		mCommandBuffers.resize(mSwapChainImageViews.size());

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = mDevice.GetCommandPool();
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

		mCommandBuffers = mDevice->allocateCommandBuffers(allocInfo);
	}

} // namespace erm
