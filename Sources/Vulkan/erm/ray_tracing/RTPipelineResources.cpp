#include "erm/ray_tracing/RTPipelineResources.h"

#include "erm/math/math.h"

#include "erm/ray_tracing/RTRenderConfigs.h"
#include "erm/ray_tracing/RTRenderData.h"
#include "erm/ray_tracing/RTShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/DeviceBindingResources.h"
#include "erm/rendering/data_structs/HostBindingResources.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

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
		CreatePipeline();
		CreateBindingTable();
		CreatePipelineData();
	}

	void RTPipelineResources::UpdateResources(vk::CommandBuffer& cmd, RTRenderData& renderData, uint32_t /*imageIndex*/)
	{
		PROFILE_FUNCTION();
		mPipelineData->UpdateResources(cmd, renderData);
	}

	void RTPipelineResources::UpdateCommandBuffer(
		vk::CommandBuffer& cmd,
		RTRenderData& /*renderData*/,
		uint32_t /*imageIndex*/)
	{
		PROFILE_FUNCTION();

		cmd.bindPipeline(vk::PipelineBindPoint::eRayTracingKHR, mPipeline.get());

		auto ds = mPipelineData->GetDescriptorSets(mEmptySet.get());

		cmd.bindDescriptorSets(
			vk::PipelineBindPoint::eRayTracingKHR,
			mPipelineLayout.get(),
			0,
			static_cast<uint32_t>(ds.size()),
			ds.data(),
			0,
			nullptr);

		mPipelineData->PostDraw();
	}

	void RTPipelineResources::CreatePipeline()
	{
		RTShaderProgram* shader = mRenderData.mRenderConfigs.mShaderProgram;

		ASSERT(shader);

		/*
			LOAD SHADERS
		*/
		std::vector<vk::UniqueShaderModule> rayGenShaderModules = shader->CreateShaderModules(ShaderType::RT_RAY_GEN);
		std::vector<vk::UniqueShaderModule> missShaderModules = shader->CreateShaderModules(ShaderType::RT_MISS);
		std::vector<vk::UniqueShaderModule> closestHitShaderModules = shader->CreateShaderModules(ShaderType::RT_CLOSEST_HIT);

		ASSERT(rayGenShaderModules.size() == 1);
		ASSERT(!missShaderModules.empty());
		ASSERT(!closestHitShaderModules.empty());

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(1 + missShaderModules.size() + closestHitShaderModules.size());

		vk::PipelineShaderStageCreateInfo& rayGenShaderStageInfo = shaderStages[0];
		rayGenShaderStageInfo.stage = vk::ShaderStageFlagBits::eRaygenKHR;
		rayGenShaderStageInfo.module = rayGenShaderModules[0].get();
		rayGenShaderStageInfo.pName = "main";

		for (size_t i = 0; i < missShaderModules.size(); ++i)
		{
			vk::PipelineShaderStageCreateInfo& missShaderStageInfo = shaderStages[i + 1];
			missShaderStageInfo.stage = vk::ShaderStageFlagBits::eMissKHR;
			missShaderStageInfo.module = missShaderModules[i].get();
			missShaderStageInfo.pName = "main";
		}

		for (size_t i = 0; i < closestHitShaderModules.size(); ++i)
		{
			vk::PipelineShaderStageCreateInfo& closestHitShaderStageInfo = shaderStages[i + 1 + missShaderModules.size()];
			closestHitShaderStageInfo.stage = vk::ShaderStageFlagBits::eClosestHitKHR;
			closestHitShaderStageInfo.module = closestHitShaderModules[i].get();
			closestHitShaderStageInfo.pName = "main";
		}

		std::vector<vk::RayTracingShaderGroupCreateInfoKHR> shaderGroups(shaderStages.size());

		vk::RayTracingShaderGroupCreateInfoKHR& rayGenGroup = shaderGroups[0];
		rayGenGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
		rayGenGroup.generalShader = 0;
		rayGenGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		rayGenGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
		rayGenGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

		for (size_t i = 0; i < missShaderModules.size(); ++i)
		{
			const uint32_t targetIndex = static_cast<uint32_t>(i) + 1;

			vk::RayTracingShaderGroupCreateInfoKHR& missGroup = shaderGroups[targetIndex];
			missGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
			missGroup.generalShader = targetIndex;
			missGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
			missGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
			missGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
		}

		for (size_t i = 0; i < closestHitShaderModules.size(); ++i)
		{
			const uint32_t targetIndex = static_cast<uint32_t>(i) + 1 + static_cast<uint32_t>(missShaderModules.size());

			vk::RayTracingShaderGroupCreateInfoKHR& closestHitGroup = shaderGroups[targetIndex];
			closestHitGroup.type = vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup;
			closestHitGroup.generalShader = VK_SHADER_UNUSED_KHR;
			closestHitGroup.closestHitShader = targetIndex;
			closestHitGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
			closestHitGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
		}

		/*
			SETUP DESCRIPTOR SET LAYOUT
		*/
		const LayoutBindingsMap& bindings = shader->GetLayoutBindingsMap();
		uint32_t maxSet = 0;
		std::for_each(bindings.begin(), bindings.end(), [&maxSet](const auto& pair) {
			maxSet = std::max(maxSet, pair.first);
		});
		mDescriptorSetLayouts.reserve(maxSet + 1);

		uint32_t maxInstanceId = 0;
		for (const auto& [id, inst] : mRenderData.mInstancesMap)
			maxInstanceId = std::max(maxInstanceId, id);

		std::map<SetIdx, std::vector<vk::DescriptorSetLayoutBinding>> layoutBindings;

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

				for (auto& layoutBinding : data)
				{
					auto& binding = layoutBindings[i].emplace_back(layoutBinding);
					if (binding.descriptorType == vk::DescriptorType::eStorageBuffer)
						binding.descriptorCount = maxInstanceId + 1;
				}

				layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings[i].size());
				layoutInfo.pBindings = layoutBindings[i].data();
			}

			mDescriptorSetLayouts.emplace_back(mDevice->createDescriptorSetLayoutUnique(layoutInfo));
		}

		vk::DescriptorSetLayoutCreateInfo emptyLayoutInfo;
		emptyLayoutInfo.bindingCount = 0;
		emptyLayoutInfo.pBindings = nullptr;

		mEmptySetLayout = mDevice->createDescriptorSetLayoutUnique(emptyLayoutInfo);

		vk::DescriptorSetAllocateInfo info {};
		info.setDescriptorPool(mDescriptorPool);
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
			CREATE PIPELINE
		*/
		vk::RayTracingPipelineCreateInfoKHR pipelineInfo = {};
		pipelineInfo.setPGroups(shaderGroups.data());
		pipelineInfo.setGroupCount(static_cast<uint32_t>(shaderGroups.size()));
		pipelineInfo.setPStages(shaderStages.data());
		pipelineInfo.setStageCount(static_cast<uint32_t>(shaderStages.size()));
		pipelineInfo.setLayout(mPipelineLayout.get());

		auto result = mDevice->createRayTracingPipelineKHRUnique(nullptr, mDevice.GetPipelineCache(), pipelineInfo);
		ASSERT(result.result == vk::Result::eSuccess);
		mPipeline = std::move(result.value);
	}

	void RTPipelineResources::CreateBindingTable()
	{
		RTShaderProgram& shader = *mRenderData.mRenderConfigs.mShaderProgram;

		uint32_t groupCount = 0;

		for (const auto& [type, data] : shader.GetShadersDataMap())
			groupCount += static_cast<uint32_t>(data.size());

		uint32_t groupHandleSize = mDevice.GetRayTracingProperties().shaderGroupHandleSize; // Size of a program identifier
		// Compute the actual size needed per SBT entry (round-up to alignment needed).
		uint32_t groupSizeAligned = math::align_up(groupHandleSize, mDevice.GetRayTracingProperties().shaderGroupBaseAlignment);
		// Bytes needed for the SBT.
		uint32_t sbtSize = groupCount * groupSizeAligned;

		// Fetch all the shader handles used in the pipeline. This is opaque data,
		// so we store it in a vector of bytes.
		std::vector<uint8_t> shaderHandleStorage(sbtSize);
		VK_CHECK(mDevice->getRayTracingShaderGroupHandlesKHR(mPipeline.get(), 0, groupCount, sbtSize, shaderHandleStorage.data()));

		// Allocate a buffer for storing the SBT.
		mSBTBuffer = std::make_unique<HostBuffer>(
			mDevice,
			sbtSize,
			vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eShaderBindingTableKHR);

		// Map the SBT buffer and write in the handles.
		void* mapped = nullptr;
		VK_CHECK(mDevice->mapMemory(mSBTBuffer->GetBufferMemory(), 0, mSBTBuffer->GetBufferSize(), {}, &mapped));
		uint8_t* pData = reinterpret_cast<uint8_t*>(mapped);
		for (uint32_t g = 0; g < groupCount; ++g)
		{
			memcpy(pData, shaderHandleStorage.data() + g * groupHandleSize, groupHandleSize);
			pData += groupSizeAligned;
		}
		mDevice->unmapMemory(mSBTBuffer->GetBufferMemory());
	}

	void RTPipelineResources::CreatePipelineData()
	{
		const RTRenderConfigs& configs = mRenderData.mRenderConfigs;
		mPipelineData = std::make_unique<PipelineData>(configs);
		const auto& sbm = configs.mShaderProgram->GetShaderBindingsMap();

		for (const auto& [set, bindings] : sbm)
		{
			BindingResources resources;
			if (set % 2 == 0)
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
			else
				resources = std::make_unique<HostBindingResources>(
					mDevice,
					mRenderer,
					set,
					mDescriptorPool,
					*configs.mShaderProgram,
					configs,
					mDescriptorSetLayouts[set].get());

			mPipelineData->AddResources(set, std::move(resources));
		}
	}

} // namespace erm
