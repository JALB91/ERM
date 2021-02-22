#include "erm/ray_tracing/RTPipelineResources.h"

#include "erm/math/math.h"

#include "erm/ray_tracing/RTRenderData.h"
#include "erm/ray_tracing/RTShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/DeviceBindingResources.h"
#include "erm/rendering/data_structs/HostBindingResources.h"

#include "erm/utils/Utils.h"

namespace erm {

	RTPipelineResources::RTPipelineResources(
		Device& device,
		IRenderer& renderer,
		const RTRenderConfigs& renderConfigs,
		const vk::DescriptorPool& descriptorPool,
		const vk::AccelerationStructureKHR* topLevelAS)
		: mDevice(device)
		, mRenderer(renderer)
		, mRenderConfigs(renderConfigs)
		, mDescriptorPool(descriptorPool)
		, mTopLevelAS(topLevelAS)
	{
		CreatePipeline();
		CreateBindingTable();
	}

	void RTPipelineResources::UpdateResources(vk::CommandBuffer& cmd, RTRenderData& renderData, uint32_t imageIndex)
	{
		auto& pipelineData = GetOrCreatePipelineData(renderData);
		pipelineData.UpdateResources(cmd, renderData);
	}

	void RTPipelineResources::UpdateCommandBuffer(vk::CommandBuffer& cmd, RTRenderData& renderData, uint32_t imageIndex)
	{
		cmd.bindPipeline(vk::PipelineBindPoint::eRayTracingKHR, mPipeline.get());

		auto& pipelineData = GetOrCreatePipelineData(renderData);
		auto ds = pipelineData.GetDescriptorSets(mEmptySet.get());

		cmd.bindDescriptorSets(
			vk::PipelineBindPoint::eRayTracingKHR,
			mPipelineLayout.get(),
			0,
			static_cast<uint32_t>(ds.size()),
			ds.data(),
			0,
			nullptr);

		pipelineData.PostDraw();
	}

	void RTPipelineResources::CreatePipeline()
	{
		RTShaderProgram* shader = mRenderConfigs.mShaderProgram;

		ASSERT(shader);

		/*
			LOAD SHADERS
		*/
		vk::ShaderModule rayGenShaderModule = shader->CreateShaderModule(ShaderType::RT_RAY_GEN);
		vk::ShaderModule missShaderModule = shader->CreateShaderModule(ShaderType::RT_MISS);
		vk::ShaderModule closestHitShaderModule = shader->CreateShaderModule(ShaderType::RT_CLOSEST_HIT);

		vk::PipelineShaderStageCreateInfo rayGenShaderStageInfo = {};
		rayGenShaderStageInfo.stage = vk::ShaderStageFlagBits::eRaygenKHR;
		rayGenShaderStageInfo.module = rayGenShaderModule;
		rayGenShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo missShaderStageInfo = {};
		missShaderStageInfo.stage = vk::ShaderStageFlagBits::eMissKHR;
		missShaderStageInfo.module = missShaderModule;
		missShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo closestHitShaderStageInfo = {};
		closestHitShaderStageInfo.stage = vk::ShaderStageFlagBits::eClosestHitKHR;
		closestHitShaderStageInfo.module = closestHitShaderModule;
		closestHitShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo shaderStages[] = {rayGenShaderStageInfo, missShaderStageInfo, closestHitShaderStageInfo};

		vk::RayTracingShaderGroupCreateInfoKHR rayGenGroup;
		rayGenGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
		rayGenGroup.generalShader = 0;
		rayGenGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		rayGenGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
		rayGenGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

		vk::RayTracingShaderGroupCreateInfoKHR missGroup;
		missGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
		missGroup.generalShader = 1;
		missGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		missGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
		missGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

		vk::RayTracingShaderGroupCreateInfoKHR closestHitGroup;
		closestHitGroup.type = vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup;
		closestHitGroup.generalShader = VK_SHADER_UNUSED_KHR;
		closestHitGroup.closestHitShader = 2;
		closestHitGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		closestHitGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

		vk::RayTracingShaderGroupCreateInfoKHR shaderGroups[] = {rayGenGroup, missGroup, closestHitGroup};

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
		pipelineInfo.setPGroups(shaderGroups);
		pipelineInfo.setGroupCount(3);
		pipelineInfo.setPStages(shaderStages);
		pipelineInfo.setStageCount(3);
		pipelineInfo.setLayout(mPipelineLayout.get());

		mPipeline = mDevice->createRayTracingPipelineKHRUnique(nullptr, mDevice.GetPipelineCache(), pipelineInfo).value;

		mDevice->destroyShaderModule(missShaderModule);
		mDevice->destroyShaderModule(rayGenShaderModule);
		mDevice->destroyShaderModule(closestHitShaderModule);
	}

	void RTPipelineResources::CreateBindingTable()
	{
		RTShaderProgram& shader = *mRenderConfigs.mShaderProgram;

		uint32_t groupCount = static_cast<uint32_t>(shader.GetShadersDataMap().size()); // 3 shaders: raygen, miss, chit
		uint32_t groupHandleSize = mDevice.GetRayTracingProperties().shaderGroupHandleSize; // Size of a program identifier
		// Compute the actual size needed per SBT entry (round-up to alignment needed).
		uint32_t groupSizeAligned = math::align_up(groupHandleSize, mDevice.GetRayTracingProperties().shaderGroupBaseAlignment);
		// Bytes needed for the SBT.
		uint32_t sbtSize = groupCount * groupSizeAligned;

		// Fetch all the shader handles used in the pipeline. This is opaque data,
		// so we store it in a vector of bytes.
		std::vector<uint8_t> shaderHandleStorage(sbtSize);
		vk::Result result = mDevice->getRayTracingShaderGroupHandlesKHR(mPipeline.get(), 0, groupCount, sbtSize, shaderHandleStorage.data());
		ASSERT(result == vk::Result::eSuccess);

		// Allocate a buffer for storing the SBT. Give it a debug name for NSight.
		mSBTBuffer = std::make_unique<HostBuffer>(
			mDevice,
			sbtSize,
			vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eShaderBindingTableKHR);

		// Map the SBT buffer and write in the handles.
		void* mapped = mDevice->mapMemory(mSBTBuffer->GetBufferMemory(), 0, mSBTBuffer->GetBufferSize());
		uint8_t* pData = reinterpret_cast<uint8_t*>(mapped);
		for (uint32_t g = 0; g < groupCount; ++g)
		{
			memcpy(pData, shaderHandleStorage.data() + g * groupHandleSize, groupHandleSize);
			pData += groupSizeAligned;
		}
		mDevice->unmapMemory(mSBTBuffer->GetBufferMemory());
	}

	PipelineData& RTPipelineResources::GetOrCreatePipelineData(RTRenderData& renderData)
	{
		auto it = std::find_if(mPipelineData.begin(), mPipelineData.end(), [&renderData](PipelineData& data) {
			return data.IsCompatible(renderData.mRenderConfigs);
		});
		if (it != mPipelineData.end())
			return *it;

		auto& data = mPipelineData.emplace_back(renderData.mRenderConfigs);
		const auto& sbm = renderData.mRenderConfigs.mShaderProgram->GetShaderBindingsMap();

		for (const auto& [set, bindings] : sbm)
		{
			BindingResources resources;
			if (set % 2 == 0)
				resources = std::make_unique<DeviceBindingResources>(
					mDevice,
					mRenderer,
					set,
					mDescriptorPool,
					*renderData.mRenderConfigs.mShaderProgram,
					renderData.mRenderConfigs,
					mDescriptorSetLayouts[set].get(),
					mTopLevelAS);
			else
				resources = std::make_unique<HostBindingResources>(
					mDevice,
					mRenderer,
					set,
					mDescriptorPool,
					*renderData.mRenderConfigs.mShaderProgram,
					renderData.mRenderConfigs,
					mDescriptorSetLayouts[set].get());

			data.AddResources(set, std::move(resources));
		}

		return data;
	}

} // namespace erm