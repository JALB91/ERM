#include "erm/rendering/data_structs/PipelineResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/data_structs/DeviceBindingResources.h"
#include "erm/rendering/data_structs/HostBindingResources.h"
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
	{}

	void PipelineResources::UpdateResources(vk::CommandBuffer& cmd, RenderData& renderData, uint32_t imageIndex)
	{
		if (mDescriptorSetLayouts.empty())
			return;

		auto& data = GetOrCreatePipelineData(renderData);
		data.UpdateResources(cmd, renderData);
	}

	void PipelineResources::UpdateCommandBuffer(vk::CommandBuffer& cmd, RenderData& renderData, uint32_t imageIndex)
	{
		if (mDescriptorSetLayouts.empty())
			return;

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

		auto& data = GetOrCreatePipelineData(renderData);
		auto ds = data.GetDescriptorSets(mEmptySet.get());

		for (const Mesh* mesh : renderData.mMeshes)
		{
			cmd.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				mPipelineLayout.get(),
				0,
				static_cast<uint32_t>(ds.size()),
				ds.data(),
				0,
				nullptr);
			/*mesh->GetVertexBuffer().Bind(cmd);
			mesh->GetIndexBuffer().Bind(cmd);
			cmd.drawIndexed(mesh->GetIndexBuffer().GetCount(), 1, 0, 0, 0);*/
		}

		data.PostDraw();
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
		vk::UniqueShaderModule vertShaderModule = shader->CreateShaderModule(ShaderType::VERTEX);
		vk::UniqueShaderModule fragShaderModule = shader->CreateShaderModule(ShaderType::FRAGMENT);

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertShaderStageInfo.module = vertShaderModule.get();
		vertShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragShaderStageInfo.module = fragShaderModule.get();
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
		const ShaderBindingsMap& bindings = shader->GetShaderBindingsMap();
		uint32_t maxSet = 0;
		std::for_each(bindings.begin(), bindings.end(), [&maxSet](const auto& pair) {
			maxSet = std::max(maxSet, pair.first);
		});
		mDescriptorSetLayouts.reserve(maxSet + 1);

		for (uint32_t i = 0; i <= maxSet; ++i)
		{
			vk::DescriptorSetLayoutCreateInfo layoutInfo {};

			if (bindings.find(i) == bindings.end())
			{
				layoutInfo.bindingCount = 0;
				layoutInfo.pBindings = nullptr;
			}
			else
			{
				auto& data = bindings.at(i);

				layoutInfo.bindingCount = static_cast<uint32_t>(data.mLayoutBindings.size());
				layoutInfo.pBindings = data.mLayoutBindings.data();
			}

			mDescriptorSetLayouts.emplace_back(mDevice->createDescriptorSetLayoutUnique(layoutInfo));
		}

		vk::DescriptorSetLayoutCreateInfo emptyLayoutInfo;
		emptyLayoutInfo.bindingCount = 0;
		emptyLayoutInfo.pBindings = nullptr;

		mEmptySetLayout = mDevice->createDescriptorSetLayoutUnique(emptyLayoutInfo);

		vk::DescriptorSetAllocateInfo info {};
		info.setDescriptorPool(*mDescriptorPool);
		info.setDescriptorSetCount(1);
		info.setPSetLayouts(&mEmptySetLayout.get());

		mEmptySet = std::move(mDevice->allocateDescriptorSetsUnique(info)[0]);

		/*
			SETUP PIPELINE LAYOUT
		*/
		std::vector<vk::DescriptorSetLayout> layouts(mDescriptorSetLayouts.size());
		for (size_t i = 0; i < mDescriptorSetLayouts.size(); ++i)
			layouts[i] = mDescriptorSetLayouts[i].get();

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
		pipelineLayoutInfo.pSetLayouts = layouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		mPipelineLayout = mDevice->createPipelineLayoutUnique(pipelineLayoutInfo);

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
		pipelineInfo.layout = mPipelineLayout.get();
		pipelineInfo.renderPass = *mRenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = nullptr;
		pipelineInfo.basePipelineIndex = -1;

		auto result = mDevice->createGraphicsPipelineUnique(mDevice.GetPipelineCache(), pipelineInfo);
		ASSERT(result.result == vk::Result::eSuccess);
		mPipeline = std::move(result.value);
	}

	PipelineData& PipelineResources::GetOrCreatePipelineData(RenderData& renderData)
	{
		auto it = std::find_if(mData.begin(), mData.end(), [&renderData](PipelineData& data) {
			return data.IsCompatible(renderData.mRenderConfigs);
		});
		if (it != mData.end())
			return *it;

		auto& data = mData.emplace_back(renderData.mRenderConfigs);
		const auto& sbm = renderData.mRenderConfigs.mShaderProgram->GetShaderBindingsMap();

		for (const auto& [set, bindings] : sbm)
		{
			BindingResources resources;
			if (set % 2 == 0)
				resources = std::make_unique<DeviceBindingResources>(
					mDevice,
					mRenderer,
					set,
					*mDescriptorPool,
					*renderData.mRenderConfigs.mShaderProgram,
					renderData.mRenderConfigs,
					renderData,
					mDescriptorSetLayouts[set].get());
			else
				resources = std::make_unique<HostBindingResources>(
					mDevice,
					mRenderer,
					set,
					*mDescriptorPool,
					*renderData.mRenderConfigs.mShaderProgram,
					renderData.mRenderConfigs,
					mDescriptorSetLayouts[set].get());

			data.AddResources(set, std::move(resources));
		}

		return data;
	}

} // namespace erm
