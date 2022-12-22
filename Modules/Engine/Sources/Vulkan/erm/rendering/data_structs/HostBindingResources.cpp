#include "erm/rendering/data_structs/HostBindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/shaders/IShaderProgram.h"

#include "erm/utils/VkUtils.h"

namespace erm {

HostBindingResources::HostBindingResources(
	Device& device,
	IRenderer& renderer,
	uint32_t targetSet,
	const vk::DescriptorPool& descriptorPool,
	const IShaderProgram& shaderProgram,
	const BindingConfigs& configs,
	const vk::DescriptorSetLayout& descriptorSetLayout)
	: IBindingResources(device, renderer, targetSet, descriptorPool, shaderProgram, configs)
{
	// CREATE DESCRIPTOR SETS
	const std::vector<vk::DescriptorSetLayout> layouts(IRenderer::kMaxFramesInFlight, descriptorSetLayout);

	vk::DescriptorSetAllocateInfo info {};
	info.setDescriptorPool(mDescriptorPool);
	info.setDescriptorSetCount(IRenderer::kMaxFramesInFlight);
	info.setPSetLayouts(layouts.data());

	ERM_VK_CHECK_AND_ASSIGN(mDescriptorSets, mDevice->allocateDescriptorSetsUnique(info));

	// GATHER SHADER DATA
	const ShaderBindingData& shaderBindings = mShaderProgram.GetShaderBindingsData(mTargetSet);
	const std::vector<UboData>& ubosData = shaderBindings.mUbosData;

	ERM_ASSERT(shaderBindings.mSamplersData.empty());
	ERM_ASSERT(shaderBindings.mStorageImagesData.empty());
	ERM_ASSERT(shaderBindings.mStorageBuffersData.empty());
#ifdef ERM_RAY_TRACING_ENABLED
	ERM_ASSERT(shaderBindings.mASData.empty());
#endif

	// UNIFORM BUFFERS
	CreateUniformBuffers(ubosData);

	// UPDATE DESCRIPTOR SETS
	std::vector<std::vector<vk::DescriptorBufferInfo>> descriptorBuffers;
	descriptorBuffers.reserve(IRenderer::kMaxFramesInFlight);
	
	std::vector<vk::WriteDescriptorSet> descriptorWrites;
	descriptorWrites.reserve(ubosData.size() * IRenderer::kMaxFramesInFlight);

	for (uint32_t i = 0; i < IRenderer::kMaxFramesInFlight; ++i)
	{
		auto& descriptorBuffer = descriptorBuffers.emplace_back();
		descriptorBuffer.reserve(ubosData.size());

		CreateUniformBuffersDescriptorWritesAndInfos(
			descriptorBuffers[i],
			descriptorWrites,
			ubosData,
			mUniformBuffers[i],
			mDescriptorSets[i].get());
	}

	mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

const vk::DescriptorSet HostBindingResources::GetDescriptorSet() const
{
	return mDescriptorSets[mRenderer.GetCurrentFrame()].get();
}

void HostBindingResources::UpdateResources(vk::CommandBuffer& /*cmd*/, IRenderData& data)
{
	for (auto& pair : mUniformBuffers[mRenderer.GetCurrentFrame()])
	{
		ERM_ASSERT(data.HasUbo(pair.first));
		pair.second.Update(data.mUbos[pair.first].get());
	}
}

void HostBindingResources::CreateUniformBuffers(const std::vector<UboData>& ubosData)
{
	for (const UboData& data : ubosData)
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
