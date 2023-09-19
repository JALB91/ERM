#include "erm/rendering/ray_tracing/RTPipelineResources.h"

#include "erm/rendering/ray_tracing/RTRenderData.h"
#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/DeviceBindingResources.h"
#include "erm/rendering/data_structs/HostBindingResources.h"
#include "erm/rendering/data_structs/PipelineData.h"
#include "erm/rendering/shaders/GPUShaderProgram.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/math/math.h>

#include <erm/utils/Profiler.h>
#include <erm/utils/Utils.h>

namespace erm {

RTPipelineResources::RTPipelineResources(
	Device& device,
	IRenderer& renderer,
	const RTRenderData& renderData,
	const vk::DescriptorPool& descriptorPool,
	const vk::AccelerationStructureKHR* topLevelAS)
	: mDevice(device)
	, mRenderer(renderer)
	, mRenderData(renderData)
	, mDescriptorPool(descriptorPool)
	, mTopLevelAS(topLevelAS)
	, mMaxInstancesCount(renderData.mInstancesMap.size())
{
	createPipeline();
	createBindingTable();
	createPipelineData();
}

RTPipelineResources::~RTPipelineResources() = default;

void RTPipelineResources::refresh()
{
	// TODO: DAMIO
	//if (mRenderData.mPipelineConfigs.mShaderProgram->needsReload())
	{
		mPipelineData.reset();
		mDescriptorSetLayouts.clear();

		createPipeline();
		createBindingTable();
		createPipelineData();

		//mRenderData.mPipelineConfigs.mShaderProgram->onReloaded();
	}
}

void RTPipelineResources::updateCommandBuffer(
	vk::CommandBuffer& cmd,
	RTRenderData& renderData)
{
	ERM_PROFILE_FUNCTION();

	mPipelineData->updateResources(cmd, renderData);

	cmd.bindPipeline(vk::PipelineBindPoint::eRayTracingKHR, mPipeline.get());

	auto ds = mPipelineData->getDescriptorSets(mEmptySet.get());

	cmd.bindDescriptorSets(
		vk::PipelineBindPoint::eRayTracingKHR,
		mPipelineLayout.get(),
		0,
		static_cast<u32>(ds.size()),
		ds.data(),
		0,
		nullptr);
}

void RTPipelineResources::createPipeline()
{
	// TODO: DAMIO
	//auto* shader = static_cast<GPUShaderProgram*>(mRenderData.mPipelineConfigs.mShaderProgram);
	GPUShaderProgram* shader = nullptr;

	ERM_ASSERT(shader);

	/*
		LOAD SHADERS
	*/
	auto rayGenShaderModules = shader->createShaderModules(ShaderType::RT_RAY_GEN);
	auto missShaderModules = shader->createShaderModules(ShaderType::RT_MISS);
	auto closestHitShaderModules = shader->createShaderModules(ShaderType::RT_CLOSEST_HIT);

	ERM_ASSERT(rayGenShaderModules.size() == 1);
	ERM_ASSERT(!missShaderModules.empty());
	ERM_ASSERT(!closestHitShaderModules.empty());

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(1 + missShaderModules.size() + closestHitShaderModules.size());

	auto& rayGenShaderStageInfo = shaderStages[0];
	rayGenShaderStageInfo.stage = vk::ShaderStageFlagBits::eRaygenKHR;
	rayGenShaderStageInfo.module = rayGenShaderModules[0].get();
	rayGenShaderStageInfo.pName = "main";

	for (u64 i = 0; i < missShaderModules.size(); ++i)
	{
		auto& missShaderStageInfo = shaderStages[i + 1];
		missShaderStageInfo.stage = vk::ShaderStageFlagBits::eMissKHR;
		missShaderStageInfo.module = missShaderModules[i].get();
		missShaderStageInfo.pName = "main";
	}

	for (u64 i = 0; i < closestHitShaderModules.size(); ++i)
	{
		auto& closestHitShaderStageInfo = shaderStages[i + 1 + missShaderModules.size()];
		closestHitShaderStageInfo.stage = vk::ShaderStageFlagBits::eClosestHitKHR;
		closestHitShaderStageInfo.module = closestHitShaderModules[i].get();
		closestHitShaderStageInfo.pName = "main";
	}

	std::vector<vk::RayTracingShaderGroupCreateInfoKHR> shaderGroups(shaderStages.size());

	auto& rayGenGroup = shaderGroups[0];
	rayGenGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
	rayGenGroup.generalShader = 0;
	rayGenGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
	rayGenGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
	rayGenGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

	for (u64 i = 0; i < missShaderModules.size(); ++i)
	{
		const u32 targetIndex = static_cast<u32>(i) + 1;

		auto& missGroup = shaderGroups[targetIndex];
		missGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
		missGroup.generalShader = targetIndex;
		missGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		missGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
		missGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
	}

	for (u64 i = 0; i < closestHitShaderModules.size(); ++i)
	{
		const u32 targetIndex = static_cast<u32>(i) + 1 + static_cast<u32>(missShaderModules.size());

		auto& closestHitGroup = shaderGroups[targetIndex];
		closestHitGroup.type = vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup;
		closestHitGroup.generalShader = VK_SHADER_UNUSED_KHR;
		closestHitGroup.closestHitShader = targetIndex;
		closestHitGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		closestHitGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
	}

	/*
		SETUP DESCRIPTOR SET LAYOUT
	*/
	const auto& bindings = shader->getLayoutBindingsMap();
	u32 maxSet = 0;
	std::for_each(bindings.begin(), bindings.end(), [&maxSet](const auto& pair) {
		maxSet = std::max(maxSet, pair.first);
	});
	mDescriptorSetLayouts.reserve(maxSet + 1);

	u32 maxInstanceId = 0;
	for (const auto& [id, inst] : mRenderData.mInstancesMap)
	{
		maxInstanceId = std::max(maxInstanceId, id);
	}

	std::map<SetIdx, std::vector<vk::DescriptorSetLayoutBinding>> layoutBindings;

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

			for (auto& layoutBinding : data)
			{
				auto& binding = layoutBindings[i].emplace_back(layoutBinding);
				if (binding.descriptorType == vk::DescriptorType::eStorageBuffer)
				{
					binding.descriptorCount = maxInstanceId + 1;
				}
			}

			layoutInfo.bindingCount = static_cast<u32>(layoutBindings[i].size());
			layoutInfo.pBindings = layoutBindings[i].data();
		}

		auto& descriptorSetLayout = mDescriptorSetLayouts.emplace_back();
		ERM_VK_CHECK_AND_ASSIGN(descriptorSetLayout, mDevice->createDescriptorSetLayoutUnique(layoutInfo));
	}

	vk::DescriptorSetLayoutCreateInfo emptyLayoutInfo;
	emptyLayoutInfo.bindingCount = 0;
	emptyLayoutInfo.pBindings = nullptr;

	ERM_VK_CHECK_AND_ASSIGN(mEmptySetLayout, mDevice->createDescriptorSetLayoutUnique(emptyLayoutInfo));

	vk::DescriptorSetAllocateInfo info {};
	info.setDescriptorPool(mDescriptorPool);
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
		CREATE PIPELINE
	*/
	vk::RayTracingPipelineCreateInfoKHR pipelineInfo = {};
	pipelineInfo.setPGroups(shaderGroups.data());
	pipelineInfo.setGroupCount(static_cast<u32>(shaderGroups.size()));
	pipelineInfo.setPStages(shaderStages.data());
	pipelineInfo.setStageCount(static_cast<u32>(shaderStages.size()));
	pipelineInfo.setLayout(mPipelineLayout.get());
	pipelineInfo.setBasePipelineHandle(mPipeline.get());

	auto result = mDevice->createRayTracingPipelineKHRUnique(nullptr, mDevice.getPipelineCache(), pipelineInfo);
	ERM_ASSERT(result.result == vk::Result::eSuccess);
	mPipeline = std::move(result.value);
}

void RTPipelineResources::createBindingTable()
{
	// TODO: DAMIO
	//const auto& rtProps = mDevice.getRayTracingProperties();
	//auto shader = mRenderData.mPipelineConfigs.mShaderProgram;

	//u32 groupCount = 0;

	//for (const auto& [type, data] : shader.getShadersDataMap())
	//{
	//	groupCount += static_cast<u32>(data.size());
	//}

	//u32 groupHandleSize = rtProps.shaderGroupHandleSize; // Size of a program identifier
	//// Compute the actual size needed per SBT entry (round-up to alignment needed).
	//u32 groupSizeAligned = math::alignUp(groupHandleSize, rtProps.shaderGroupBaseAlignment);
	//// Bytes needed for the SBT.
	//u32 sbtSize = groupCount * groupSizeAligned;

	//// Fetch all the shader handles used in the pipeline. This is opaque data,
	//// so we store it in a vector of bytes.
	//std::vector<u8> shaderHandleStorage(sbtSize);
	//ERM_VK_CHECK(mDevice->getRayTracingShaderGroupHandlesKHR(mPipeline.get(), 0, groupCount, sbtSize, shaderHandleStorage.data()));

	//if (!mSBTBuffer || mSBTBuffer->getBufferSize() != sbtSize)
	//{
	//	// Allocate a buffer for storing the SBT.
	//	mSBTBuffer = std::make_unique<HostBuffer>(
	//		mDevice,
	//		sbtSize,
	//		BufferUsage::TRANSFER_SRC | BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::SHADER_BINDING_TABLE);
	//}

	//// Map the SBT buffer and write in the handles.
	//void* mapped = nullptr;
	//ERM_VK_CHECK(mDevice->mapMemory(mSBTBuffer->getBufferMemory(), 0, mSBTBuffer->getBufferSize(), {}, &mapped));
	//u8* pData = reinterpret_cast<u8*>(mapped);
	//for (u32 g = 0; g < groupCount; ++g)
	//{
	//	memcpy(pData, shaderHandleStorage.data() + g * groupHandleSize, groupHandleSize);
	//	pData += groupSizeAligned;
	//}
	//mDevice->unmapMemory(mSBTBuffer->getBufferMemory());
}

void RTPipelineResources::createPipelineData()
{
	/*const auto& configs = mRenderData.mPipelineConfigs;
	mPipelineData = std::make_unique<PipelineData>(configs);
	const auto& sbm = configs.mShaderProgram->getShaderBindingsMap();

	for (const auto& [set, bindings] : sbm)
	{
		BindingResources resources;
		if (set % 2 == 0)
		{
			resources = std::make_unique<DeviceBindingResources>(
				mDevice,
				mRenderer,
				set,
				mDescriptorPool,
				*configs.mShaderProgram,
				configs,
				mRenderData,
				mDescriptorSetLayouts[set].get(),
				mTopLevelAS);
		}
		else
		{
			resources = std::make_unique<HostBindingResources>(
				mDevice,
				mRenderer,
				set,
				mDescriptorPool,
				*configs.mShaderProgram,
				configs,
				mDescriptorSetLayouts[set].get());
		}

		mPipelineData->addResources(set, std::move(resources));
	}*/
}

} // namespace erm
