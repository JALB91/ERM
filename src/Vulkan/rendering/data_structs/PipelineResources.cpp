#include "erm/rendering/data_structs/PipelineResources.h"

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

	PipelineResources::PipelineResources(
		Device& device,
		Renderer& renderer,
		const vk::RenderPass* renderPass,
		const vk::DescriptorPool* descriptorPool,
		const RenderConfigs& renderConfigs)
		: mDevice(device)
		, mRenderer(renderer)
		, mRenderPass(renderPass)
		, mDescriptorPool(descriptorPool)
		, mRenderConfigs(renderConfigs)
	{
		CreatePipeline();
	}

	PipelineResources::~PipelineResources()
	{
		mDevice->destroyDescriptorSetLayout(mDescriptorSetLayout);
		mBindingResources.clear();
		mDevice->destroyPipelineLayout(mPipelineLayout);
	}

	void PipelineResources::Update(vk::CommandBuffer& cmd, RenderData& renderData, uint32_t imageIndex)
	{
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline.get());

#ifdef ERM_FLIP_VIEWPORT
		const BoundingBox2D& normViewport = mRenderConfigs.GetNormViewport();
		const math::vec2 normViewportSize = normViewport.GetSize();
		const vk::Extent2D& extent = mRenderer.GetSwapChainExtent();

		vk::Viewport viewport = {};
		viewport.x = static_cast<float>(extent.width) * normViewport.mMin.x;
		viewport.y = (static_cast<float>(extent.height) * normViewportSize.y) - (static_cast<float>(extent.height) * normViewport.mMin.y);
		viewport.width = static_cast<float>(extent.width) * normViewportSize.x;
		viewport.height = -(static_cast<float>(extent.height) * normViewportSize.y);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		cmd.setViewport(0, 1, &viewport);
#endif

		vk::DescriptorSet* descriptorSet = nullptr;

		if (mDescriptorSetLayout)
		{
			if (!renderData.mRenderingId.has_value() || renderData.mRenderingId.value() >= static_cast<uint32_t>(mBindingResources.size()))
			{
				renderData.mRenderingId = static_cast<uint32_t>(mBindingResources.size());
				mBindingResources.emplace_back(mDevice, mRenderer, *mDescriptorPool, mDescriptorSetLayout, renderData.mRenderConfigs);
			}

			BindingResources& resources = mBindingResources[renderData.mRenderingId.value()];
			resources.UpdateResources(renderData, imageIndex);

			descriptorSet = resources.GetDescriptorSet(imageIndex);
		}

		for (const Mesh* mesh : renderData.mMeshes)
		{
			if (descriptorSet)
				cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout, 0, 1, descriptorSet, 0, nullptr);
			mesh->GetVertexBuffer().Bind(cmd);
			mesh->GetIndexBuffer().Bind(cmd);
			cmd.drawIndexed(mesh->GetIndexBuffer().GetCount(), 1, 0, 0, 0);
		}
	}

	void PipelineResources::CreatePipeline()
	{
		const BoundingBox2D& normViewport = mRenderConfigs.GetNormViewport();
		const math::vec2 normViewportSize = normViewport.GetSize();
		const vk::Extent2D& extent = mRenderer.GetSwapChainExtent();

		ShaderProgram* shader = mRenderConfigs.mShaderProgram;

		ASSERT(shader);

		/*
			LOAD SHADERS
		*/
		vk::ShaderModule vertShaderModule = shader->CreateVertexShaderModule();
		vk::ShaderModule fragShaderModule = shader->CreateFragmentShaderModule();

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
		vk::VertexInputBindingDescription bindingDescription = shader->GetVertexBindingDescription();
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = shader->GetVertexAttributeDescriptions();

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		/*
			SETUP INPUT ASSEMBLY
		*/
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.topology = VkUtils::ToVulkanValue<vk::PrimitiveTopology>(mRenderConfigs.GetDrawMode());
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		/*
			SETUP VIEWPORT AND SCISSOR
		*/
		vk::Viewport viewport = {};
		viewport.x = static_cast<float>(extent.width) * normViewport.mMin.x;
		viewport.y = static_cast<float>(extent.height) * normViewport.mMin.y;
		viewport.width = static_cast<float>(extent.width) * normViewportSize.x;
		viewport.height = static_cast<float>(extent.height) * normViewportSize.y;
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
		rasterizer.polygonMode = VkUtils::ToVulkanValue<vk::PolygonMode>(mRenderConfigs.GetPolygonMode());
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VkUtils::ToVulkanValue<vk::CullModeFlagBits>(mRenderConfigs.GetCullMode());
		rasterizer.frontFace = VkUtils::ToVulkanValue<vk::FrontFace>(mRenderConfigs.GetFrontFace());
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
		colorBlendAttachment.blendEnable = mRenderConfigs.GetBlendEnabled() ? VK_TRUE : VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
		colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
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
		std::vector<vk::DescriptorSetLayoutBinding> bindings = shader->GetDescriptorSetLayoutBindings();

		if (!bindings.empty())
		{
			vk::DescriptorSetLayoutCreateInfo layoutInfo {};
			layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			layoutInfo.pBindings = bindings.data();

			mDescriptorSetLayout = mDevice->createDescriptorSetLayout(layoutInfo);
		}
		else
		{
			mDescriptorSetLayout = nullptr;
		}

		/*
			SETUP PIPELINE LAYOUT
		*/
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.setLayoutCount = mDescriptorSetLayout ? 1 : 0;
		pipelineLayoutInfo.pSetLayouts = mDescriptorSetLayout ? &mDescriptorSetLayout : nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		mPipelineLayout = mDevice->createPipelineLayout(pipelineLayoutInfo);

		/*
			SETUP STENCIL AND DEPTH TESTS
		*/
		vk::PipelineDepthStencilStateCreateInfo depthStencil {};
		depthStencil.depthTestEnable = mRenderConfigs.GetDepthTestEnabled() ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = mRenderConfigs.GetDepthWriteEnabled() ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = VkUtils::ToVulkanValue<vk::CompareOp>(mRenderConfigs.GetDepthFunction());
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;

		/*
			DYNAMIC STATES
		*/
		std::vector<vk::DynamicState> dynamicStates {
#ifdef ERM_FLIP_VIEWPORT
			vk::DynamicState::eViewport
#endif
		};

		vk::PipelineDynamicStateCreateInfo dynamicInfo;
		dynamicInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicInfo.pDynamicStates = dynamicStates.data();

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
		pipelineInfo.pDynamicState = &dynamicInfo;
		pipelineInfo.layout = mPipelineLayout;
		pipelineInfo.renderPass = *mRenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = nullptr;
		pipelineInfo.basePipelineIndex = -1;

		mPipeline = mDevice->createGraphicsPipelineUnique(mDevice.GetPipelineCache(), pipelineInfo).value;

		mDevice->destroyShaderModule(vertShaderModule);
		mDevice->destroyShaderModule(fragShaderModule);
	}

} // namespace erm
