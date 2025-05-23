#include "erm/rendering/data_structs/HostBindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/shaders/IShaderProgram.h"
#include "erm/rendering/utils/VkUtils.h"

namespace erm {

HostBindingResources::HostBindingResources(
	Device& device,
	Renderer& renderer,
	u32 targetSet,
	const IShaderProgram& shaderProgram,
	const BindingConfigs& configs,
	vk::DescriptorPool descriptorPool,
	vk::DescriptorSetLayout descriptorSetLayout)
	: IBindingResources(device, renderer, targetSet, shaderProgram, configs, descriptorPool)
{
	// CREATE DESCRIPTOR SETS
	const std::vector<vk::DescriptorSetLayout> layouts(Renderer::kMaxFramesInFlight, descriptorSetLayout);

	vk::DescriptorSetAllocateInfo info {};
	info.setDescriptorPool(mDescriptorPool);
	info.setDescriptorSetCount(Renderer::kMaxFramesInFlight);
	info.setPSetLayouts(layouts.data());

	ERM_VK_CHECK_AND_ASSIGN(mDescriptorSets, mDevice->allocateDescriptorSetsUnique(info));

	// GATHER SHADER DATA
//	TODO: Damiano
//	const ShaderBindingData& shaderBindings = mShaderProgram.getShaderBindingsData(mTargetSet);
	ShaderBindingData shaderBindings;
	const std::vector<UboData>& ubosData = shaderBindings.mUbosData;

	ERM_ASSERT(shaderBindings.mSamplersData.empty());
	ERM_ASSERT(shaderBindings.mStorageImagesData.empty());
	ERM_ASSERT(shaderBindings.mStorageBuffersData.empty());
#ifdef ERM_RAY_TRACING_ENABLED
	ERM_ASSERT(shaderBindings.mASData.empty());
#endif

	// UNIFORM BUFFERS
	createUniformBuffers(ubosData);

	// UPDATE DESCRIPTOR SETS
	std::vector<std::vector<vk::DescriptorBufferInfo>> descriptorBuffers;
	descriptorBuffers.reserve(Renderer::kMaxFramesInFlight);
	
	std::vector<vk::WriteDescriptorSet> descriptorWrites;
	descriptorWrites.reserve(ubosData.size() * Renderer::kMaxFramesInFlight);

	for (u32 i = 0; i < Renderer::kMaxFramesInFlight; ++i)
	{
		auto& descriptorBuffer = descriptorBuffers.emplace_back();
		descriptorBuffer.reserve(ubosData.size());

		createUniformBuffersDescriptorWritesAndInfos(
			descriptorBuffers[i],
			descriptorWrites,
			ubosData,
			mUniformBuffers[i],
			mDescriptorSets[i].get());
	}

	mDevice->updateDescriptorSets(static_cast<u32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

vk::DescriptorSet HostBindingResources::getDescriptorSet() const
{
	return mDescriptorSets[mRenderer.getCurrentFrame()].get();
}

void HostBindingResources::updateResources(vk::CommandBuffer /*cmd*/, IRenderData& data)
{
	for (auto& pair : mUniformBuffers[mRenderer.getCurrentFrame()])
	{
		ERM_ASSERT(data.hasUbo(pair.first));
		pair.second.update(data.mUbos[pair.first].get());
	}
}

void HostBindingResources::createUniformBuffers(const std::vector<UboData>& ubosData)
{
	for (const auto& data : ubosData)
	{
		for (auto& buffersMap : mUniformBuffers)
		{
			buffersMap.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(data.mUboId),
				std::forward_as_tuple(mDevice, data.mStride));
		}
	}
}

} // namespace erm
