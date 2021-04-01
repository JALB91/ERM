#include "erm/rendering/data_structs/HostBindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/renderer/IRenderer.h"
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
	, mCurrentBufferIndex(0)
{
	mUniformBuffers.resize(IRenderer::kMaxFramesInFlight);

	// CREATE DESCRIPTOR SETS
	const std::vector<vk::DescriptorSetLayout> layouts(IRenderer::kMaxFramesInFlight, descriptorSetLayout);

	vk::DescriptorSetAllocateInfo info {};
	info.setDescriptorPool(mDescriptorPool);
	info.setDescriptorSetCount(IRenderer::kMaxFramesInFlight);
	info.setPSetLayouts(layouts.data());

	mDescriptorSets = mDevice->allocateDescriptorSetsUnique(info);

	// GATHER SHADER DATA
	const ShaderBindingData& shaderBindings = mShaderProgram.GetShaderBindingsData(mTargetSet);
	const std::vector<UboData>& ubosData = shaderBindings.mUbosData;

	ASSERT(shaderBindings.mSamplersData.empty());
	ASSERT(shaderBindings.mStorageImagesData.empty());
	ASSERT(shaderBindings.mStorageBuffersData.empty());
#ifdef ERM_RAY_TRACING_ENABLED
	ASSERT(shaderBindings.mASData.empty());
#endif

	// UNIFORM BUFFERS
	CreateUniformBuffers(ubosData);

	// UPDATE DESCRIPTOR SETS
	std::vector<std::vector<vk::DescriptorBufferInfo>> descriptorBuffers(IRenderer::kMaxFramesInFlight);
	std::vector<vk::WriteDescriptorSet> descriptorWrites(ubosData.size() * IRenderer::kMaxFramesInFlight);

	for (uint32_t i = 0; i < IRenderer::kMaxFramesInFlight; ++i)
	{
		descriptorBuffers[i].resize(ubosData.size());

		CreateUniformBuffersDescriptorWritesAndInfos(
			descriptorBuffers[i],
			descriptorWrites,
			ubosData,
			mUniformBuffers[i],
			mDescriptorSets[i].get(),
			static_cast<uint32_t>(ubosData.size()) * i);
	}

	mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

const vk::DescriptorSet HostBindingResources::GetDescriptorSet() const
{
	return mDescriptorSets[mCurrentBufferIndex].get();
}

void HostBindingResources::UpdateResources(vk::CommandBuffer& /*cmd*/, IRenderData& data)
{
	for (auto& pair : mUniformBuffers[mCurrentBufferIndex])
	{
		ASSERT(data.HasUbo(pair.first));
		pair.second.Update(data.mUbos[pair.first].get());
	}
}

void HostBindingResources::PostDraw()
{
	mCurrentBufferIndex = (mCurrentBufferIndex + 1) % IRenderer::kMaxFramesInFlight;
}

void HostBindingResources::CreateUniformBuffers(const std::vector<UboData>& ubosData)
{
	for (const UboData& data : ubosData)
	{
		for (UniformBuffers& buffs : mUniformBuffers)
		{
			buffs.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(data.mUboId),
				std::forward_as_tuple(mDevice, data.mStride));
		}
	}
}

} // namespace erm
