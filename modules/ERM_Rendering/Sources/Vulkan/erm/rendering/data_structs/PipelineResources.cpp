#include "erm/rendering/data_structs/PipelineResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/DeviceBindingResources.h"
#include "erm/rendering/data_structs/HostBindingResources.h"
#include "erm/rendering/models/GPUMesh.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/shaders/GPUShaderProgram.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/assets/AssetsRepo.h>

#include <erm/window/Window.h>

#include <erm/utils/Utils.h>

namespace erm {

PipelineResources::PipelineResources(
	Device& device,
	IWindow& window,
	Renderer& renderer,
	const vk::RenderPass* renderPass,
	const vk::DescriptorPool* descriptorPool,
	const PipelineConfigs& pipelineConfigs)
: mDevice(device)
, mWindow(window)
, mRenderer(renderer)
, mRenderPass(renderPass)
, mDescriptorPool(descriptorPool)
, mPipelineConfigs(pipelineConfigs)
, mUntouchedFrames(0)
{
	createPipeline();
}

PipelineResources::~PipelineResources() = default;

PipelineResources& PipelineResources::operator=(PipelineResources&& other) noexcept
{
	ERM_ASSERT(mRenderPass == other.mRenderPass);
	ERM_ASSERT(mDescriptorPool == other.mDescriptorPool);
	
	mPipelineConfigs = std::move(other.mPipelineConfigs);
	mPipelineLayout = std::move(other.mPipelineLayout);
	mPipeline = std::move(other.mPipeline);
	mEmptySetLayout = std::move(other.mEmptySetLayout);
	mEmptySet = std::move(other.mEmptySet);
	mDescriptorSetLayouts = std::move(other.mDescriptorSetLayouts);
	mPipelineData = std::move(other.mPipelineData);
	mUntouchedFrames = std::move(other.mUntouchedFrames);
	
	return *this;
}

void PipelineResources::refresh()
{
//	TODO: Damiano
//	if (mPipelineConfigs.mShaderProgram->needsReload())
//	{
//		mPipelineData.clear();
//		mDescriptorSetLayouts.clear();
//
//		createPipeline();
//
//		mPipelineConfigs.mShaderProgram->onReloaded();
//	}
	
	for (auto it = mPipelineData.begin(); it != mPipelineData.end();)
	{
		auto& [id, data] = *it;
		
		if (data.getUntouchedFrames() > Renderer::kMaxFramesInFlight)
		{
			it = mPipelineData.erase(it);
		}
		else
		{
			data.refresh();
			++it;
		}
	}
	
	++mUntouchedFrames;
}

void PipelineResources::updateResources(vk::CommandBuffer& cmd, RenderData& renderData)
{
	if (mDescriptorSetLayouts.empty())
	{
		return;
	}
	
	mUntouchedFrames = 0;

	auto& data = getOrCreatePipelineData(renderData);
	data.updateResources(cmd, renderData);
}

void PipelineResources::updateCommandBuffer(vk::CommandBuffer& cmd, RenderData& renderData)
{
	if (mDescriptorSetLayouts.empty())
	{
		return;
	}	

	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline.get());

#ifdef ERM_FLIP_VIEWPORT
	const auto& normViewport = mWindow.getNormalizedViewport();
	const auto& normViewportSize = normViewport.getSize();
	const auto& extent = mRenderer.getSwapChainExtent();

	vk::Viewport viewport = {};
	viewport.x = static_cast<float>(extent.width) * normViewport.mMin.x;
	viewport.y = (static_cast<float>(extent.height) * normViewportSize.y) - (static_cast<float>(extent.height) * normViewport.mMin.y);
	viewport.width = static_cast<float>(extent.width) * normViewportSize.x;
	viewport.height = -(static_cast<float>(extent.height) * normViewportSize.y);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	cmd.setViewport(0, 1, &viewport);
#endif

	ERM_ASSERT(renderData.mBindingId.has_value() && mPipelineData.find(renderData.mBindingId.value()) != mPipelineData.end());
	auto& data = mPipelineData.at(renderData.mBindingId.value());
	auto ds = data.getDescriptorSets(mEmptySet.get());

	cmd.bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics,
		mPipelineLayout.get(),
		0,
		static_cast<u32>(ds.size()),
		ds.data(),
		0,
		nullptr);

//	TODO: Damiano
//	auto assetsRepo = gServiceLocator.get<AssetsRepo>();
//	for (u64 i = 0; i < renderData.mMeshes.size(); ++i)
//	{
//		Mesh* mesh = assetsRepo->get<Mesh>(renderData.mMeshes[i]);
//		
//		if (mesh == nullptr)
//			continue;
//		
//		const BufferHandle& vHandle = mesh->getVertBufferHandle();
//		const BufferHandle& iHandle = mesh->getIndBufferHandle();
//
//		vk::Buffer vBuffers[] = {vHandle.mBuffer};
//		vk::DeviceSize offsets[] = {0};
//
//		cmd.bindVertexBuffers(0, 1, vBuffers, offsets);
//		cmd.bindIndexBuffer(iHandle.mBuffer, iHandle.mInfo.mOffset, vk::IndexType::eUint32);
//		cmd.drawIndexed(static_cast<u32>(mesh.getIndicesData().size()), 1, 0, 0, 0);
//	}
}

void PipelineResources::createPipeline()
{
	const auto& normViewport = mWindow.getNormalizedViewport();
	const auto& normViewportSize = normViewport.getSize();
	const auto& extent = mRenderer.getSwapChainExtent();

//	auto* shaderProgram = gAssetsLib.getAssetsRepo().getAsset<IShaderProgram>(mPipelineConfigs.mShaderProgram);
	auto* shader = static_cast<GPUShaderProgram*>(nullptr);

	ERM_ASSERT(shader != nullptr);

	/*
		LOAD SHADERS
	*/
	auto vertShaderModules = shader->createShaderModules(ShaderType::VERTEX);
	auto fragShaderModules = shader->createShaderModules(ShaderType::FRAGMENT);

	ERM_ASSERT(vertShaderModules.size() == 1);
	ERM_ASSERT(fragShaderModules.size() == 1);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertShaderStageInfo.module = vertShaderModules[0].get();
	vertShaderStageInfo.pName = "main";

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragShaderStageInfo.module = fragShaderModules[0].get();
	fragShaderStageInfo.pName = "main";

	vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	/*
		SETUP VERTEX INPUT
	*/
	auto bindingDescription = shader->getVertexBindingDescription();
	auto attributeDescriptions = shader->getVertexAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<u32>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	/*
		SETUP INPUT ASSEMBLY
	*/
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.topology = VkUtils::toVulkanValue<vk::PrimitiveTopology>(mPipelineConfigs.getDrawMode());
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
	scissor.offset = vk::Offset2D(static_cast<u32>(viewport.x), static_cast<u32>(viewport.y));
	scissor.extent = vk::Extent2D(static_cast<u32>(viewport.width), static_cast<u32>(viewport.height));

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
	rasterizer.polygonMode = VkUtils::toVulkanValue<vk::PolygonMode>(mPipelineConfigs.getPolygonMode());
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VkUtils::toVulkanValue<vk::CullModeFlagBits>(mPipelineConfigs.getCullMode());
	rasterizer.frontFace = VkUtils::toVulkanValue<vk::FrontFace>(mPipelineConfigs.getFrontFace());
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
	colorBlendAttachment.blendEnable = mPipelineConfigs.getBlendEnabled() ? VK_TRUE : VK_FALSE;
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
	const auto& bindings = shader->getLayoutBindingsMap();
	u32 maxSet = 0;
	std::for_each(bindings.begin(), bindings.end(), [&maxSet](const auto& pair) {
		maxSet = std::max(maxSet, pair.first);
	});
	mDescriptorSetLayouts.reserve(maxSet + 1);

	for (u32 i = 0; i <= maxSet; ++i)
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

			layoutInfo.bindingCount = static_cast<u32>(data.size());
			layoutInfo.pBindings = data.data();
		}

		auto& descriptorSetLayout = mDescriptorSetLayouts.emplace_back();
		ERM_VK_CHECK_AND_ASSIGN(descriptorSetLayout, mDevice->createDescriptorSetLayoutUnique(layoutInfo));
	}

	vk::DescriptorSetLayoutCreateInfo emptyLayoutInfo;
	emptyLayoutInfo.bindingCount = 0;
	emptyLayoutInfo.pBindings = nullptr;

	ERM_VK_CHECK_AND_ASSIGN(mEmptySetLayout, mDevice->createDescriptorSetLayoutUnique(emptyLayoutInfo));

	vk::DescriptorSetAllocateInfo info {};
	info.setDescriptorPool(*mDescriptorPool);
	info.setDescriptorSetCount(1);
	info.setPSetLayouts(&mEmptySetLayout.get());

	{
		auto result = mDevice->allocateDescriptorSetsUnique(info);
		ERM_ASSERT(result.result == vk::Result::eSuccess);
		mEmptySet = std::move(result.value[0]);
	}

	/*
		SETUP PIPELINE LAYOUT
	*/
	std::vector<vk::DescriptorSetLayout> layouts(mDescriptorSetLayouts.size());
	for (u64 i = 0; i < mDescriptorSetLayouts.size(); ++i)
	{
		layouts[i] = mDescriptorSetLayouts[i].get();
	}

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.setLayoutCount = static_cast<u32>(layouts.size());
	pipelineLayoutInfo.pSetLayouts = layouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	ERM_VK_CHECK_AND_ASSIGN(mPipelineLayout, mDevice->createPipelineLayoutUnique(pipelineLayoutInfo));

	/*
		SETUP STENCIL AND DEPTH TESTS
	*/
	vk::PipelineDepthStencilStateCreateInfo depthStencil {};
	depthStencil.depthTestEnable = mPipelineConfigs.getDepthTestEnabled() ? VK_TRUE : VK_FALSE;
	depthStencil.depthWriteEnable = mPipelineConfigs.getDepthWriteEnabled() ? VK_TRUE : VK_FALSE;
	depthStencil.depthCompareOp = VkUtils::toVulkanValue<vk::CompareOp>(mPipelineConfigs.getDepthFunction());
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
	dynamicInfo.dynamicStateCount = static_cast<u32>(dynamicStates.size());
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
	pipelineInfo.basePipelineHandle = mPipeline.get();
	pipelineInfo.basePipelineIndex = -1;

	auto result = mDevice->createGraphicsPipelineUnique(mDevice.getPipelineCache(), pipelineInfo);
	ERM_ASSERT(result.result == vk::Result::eSuccess);
	mPipeline = std::move(result.value);
}

PipelineData& PipelineResources::getOrCreatePipelineData(RenderData& renderData)
{
	if (renderData.mBindingId.has_value())
	{
		const auto it = mPipelineData.find(renderData.mBindingId.value());
		if (it != mPipelineData.end())
		{
			return it->second;
		}
		else
		{
			renderData.mBindingId.reset();
		}
	}
	
	for (u32 i = 0; i < static_cast<u32>(mPipelineData.size()); ++i)
	{
		if (mPipelineData.find(i) == mPipelineData.end())
		{
			renderData.mBindingId = i;
			break;
		}
	}

	if (!renderData.mBindingId.has_value())
	{
		renderData.mBindingId = static_cast<u32>(mPipelineData.size());
	}

	auto result = mPipelineData.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(renderData.mBindingId.value()),
		std::forward_as_tuple(renderData.mPipelineConfigs));
	auto& data = result.first->second;
//	TODO: Damiano
//	const auto& sbm = renderData.mPipelineConfigs.mShaderProgram->getShaderBindingsMap();
	ShaderBindingsMap sbm;
	IShaderProgram* sp = nullptr;

	for (const auto& [set, bindings] : sbm)
	{
		BindingResources resources;
		if (set % 2 == 0)
			resources = std::make_unique<DeviceBindingResources>(
				mDevice,
				mRenderer,
				set,
//				*renderData.mPipelineConfigs.mShaderProgram,
																 *sp,
				renderData.mPipelineConfigs,
				renderData,
				*mDescriptorPool,
				mDescriptorSetLayouts[set].get());
		else
			resources = std::make_unique<HostBindingResources>(
				mDevice,
				mRenderer,
				set,
//				*renderData.mPipelineConfigs.mShaderProgram,
															   *sp,
				renderData.mPipelineConfigs,
				*mDescriptorPool,
				mDescriptorSetLayouts[set].get());

		data.addResources(set, std::move(resources));
	}

	return data;
}

} // namespace erm
