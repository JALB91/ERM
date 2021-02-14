#include "erm/ray_tracing/RTRenderingResources.h"

#include "erm/ray_tracing/RTShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/IRenderer.h"

#include "erm/utils/VkUtils.h"

namespace erm {

	RTRenderingResources::RTRenderingResources(
		Device& device,
		IRenderer& renderer,
		const RTRenderConfigs& renderConfigs)
		: mDevice(device)
		, mRenderer(renderer)
		, mRenderConfigs(renderConfigs)
	{
		CreatePipeline();
	}

	RTRenderingResources::~RTRenderingResources()
	{
		mDevice->destroyDescriptorSetLayout(mDescriptorSetLayout);
	}

	vk::CommandBuffer RTRenderingResources::UpdateCommandBuffer(std::vector<RenderData*>& renderData, uint32_t imageIndex)
	{
		vk::CommandBuffer& cmd = mCommandBuffers[imageIndex];
		cmd.reset({});

		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = {};
		beginInfo.pInheritanceInfo = nullptr;

		cmd.begin(beginInfo);

		cmd.bindPipeline(vk::PipelineBindPoint::eRayTracingKHR, mPipeline.get());
		//cmd.bindDescriptorSets(vk::PipelineBindPoint::eRayTracingKHR, mPipelineLayout.get(), 0,);
		//cmd.traceRaysKHR();

		cmd.end();

		return cmd;
	}

	void RTRenderingResources::Refresh()
	{
		if (mRenderConfigs.mShaderProgram->NeedsReload())
			Reload();

		mRenderConfigs.mShaderProgram->OnReloaded();
	}

	void RTRenderingResources::Reload()
	{
		Cleanup();
		CreatePipeline();
		CreateDescriptorPool();
		CreateCommandBuffers();
	}

	void RTRenderingResources::Cleanup()
	{
		mDevice->freeCommandBuffers(mDevice.GetCommandPool(), static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());
		mCommandBuffers.clear();
		mPipeline = {};
		mPipelineLayout = {};
		mDevice->destroyDescriptorPool(mDescriptorPool);
		mDescriptorPool = nullptr;
	}

	void RTRenderingResources::CreatePipeline()
	{
		RTShaderProgram* shader = mRenderConfigs.mShaderProgram;

		ASSERT(shader);

		/*
			LOAD SHADERS
		*/
		vk::ShaderModule rayGenShaderModule = shader->CreateShaderModule(ShaderType::RT_RAY_GEN);
		vk::ShaderModule anyHitShaderModule = shader->CreateShaderModule(ShaderType::RT_ANY_HIT);
		vk::ShaderModule closestHitShaderModule = shader->CreateShaderModule(ShaderType::RT_CLOSEST_HIT);

		vk::PipelineShaderStageCreateInfo rayGenShaderStageInfo = {};
		rayGenShaderStageInfo.stage = vk::ShaderStageFlagBits::eRaygenKHR;
		rayGenShaderStageInfo.module = rayGenShaderModule;
		rayGenShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo anyHitShaderStageInfo = {};
		anyHitShaderStageInfo.stage = vk::ShaderStageFlagBits::eAnyHitKHR;
		anyHitShaderStageInfo.module = anyHitShaderModule;
		anyHitShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo closestHitShaderStageInfo = {};
		closestHitShaderStageInfo.stage = vk::ShaderStageFlagBits::eClosestHitKHR;
		closestHitShaderStageInfo.module = closestHitShaderModule;
		closestHitShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo shaderStages[] = {rayGenShaderStageInfo, anyHitShaderStageInfo, closestHitShaderStageInfo};

		vk::RayTracingShaderGroupCreateInfoKHR shaderGroupInfo = {};
		shaderGroupInfo.generalShader = 0;
		shaderGroupInfo.anyHitShader = 1;
		shaderGroupInfo.closestHitShader = 2;

		/*
			SETUP DESCRIPTOR SET LAYOUT
		*/
		std::vector<vk::DescriptorSetLayoutBinding> bindings = shader->GetDescriptorSetLayoutBindings();
		ASSERT(!bindings.empty());

		vk::DescriptorSetLayoutCreateInfo layoutInfo {};
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		mDescriptorSetLayout = mDevice->createDescriptorSetLayout(layoutInfo);

		/*
			SETUP PIPELINE LAYOUT
		*/
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.setLayoutCount = mDescriptorSetLayout ? 1 : 0;
		pipelineLayoutInfo.pSetLayouts = mDescriptorSetLayout ? &mDescriptorSetLayout : nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		mPipelineLayout = mDevice->createPipelineLayoutUnique(pipelineLayoutInfo);

		/*
			CREATE PIPELINE
		*/
		vk::RayTracingPipelineCreateInfoKHR pipelineInfo = {};
		pipelineInfo.setPGroups(&shaderGroupInfo);
		pipelineInfo.setGroupCount(1);
		pipelineInfo.setPStages(shaderStages);
		pipelineInfo.setStageCount(3);
		pipelineInfo.setLayout(mPipelineLayout.get());

		mPipeline = mDevice->createRayTracingPipelineKHRUnique(nullptr, mDevice.GetPipelineCache(), pipelineInfo).value;

		mDevice->destroyShaderModule(anyHitShaderModule);
		mDevice->destroyShaderModule(rayGenShaderModule);
		mDevice->destroyShaderModule(closestHitShaderModule);
	}

	void RTRenderingResources::CreateDescriptorPool()
	{
		const std::vector<vk::ImageView>& swapChainImageViews = mRenderer.GetSwapChainImageViews();

		std::array<vk::DescriptorPoolSize, 3> poolSizes {};
		poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImageViews.size() * 1000);
		poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImageViews.size() * 100);
		poolSizes[2].type = vk::DescriptorType::eStorageImage;
		poolSizes[2].descriptorCount = static_cast<uint32_t>(swapChainImageViews.size() * 10);

		vk::DescriptorPoolCreateInfo poolInfo {};
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(swapChainImageViews.size() * 100);
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

		mDescriptorPool = mDevice->createDescriptorPool(poolInfo);
	}

	void RTRenderingResources::CreateCommandBuffers()
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
