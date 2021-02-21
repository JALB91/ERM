#include "erm/rendering/data_structs/HostBindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/IRenderer.h"
#include "erm/rendering/shaders/ShaderProgram.h"

namespace erm {

	HostBindingResources::HostBindingResources(
		Device& device,
		IRenderer& renderer,
		uint32_t targetSet,
		const vk::DescriptorPool& descriptorPool,
		const RenderConfigs& renderConfigs,
		const vk::DescriptorSetLayout& descriptorSetLayout)
		: IBindingResources(device, renderer, targetSet, descriptorPool, renderConfigs)
		, mCurrentBufferIndex(0)
	{
		const ShaderProgram* shader = mRenderConfigs.mShaderProgram;
		ASSERT(shader);

		mUniformBuffers.resize(IRenderer::kMaxFramesInFlight);

		// CREATE DESCRIPTOR SETS
		const std::vector<vk::DescriptorSetLayout> layouts(IRenderer::kMaxFramesInFlight, descriptorSetLayout);

		vk::DescriptorSetAllocateInfo info {};
		info.setDescriptorPool(mDescriptorPool);
		info.setDescriptorSetCount(IRenderer::kMaxFramesInFlight);
		info.setPSetLayouts(layouts.data());

		mDescriptorSets.resize(IRenderer::kMaxFramesInFlight);
		mDescriptorSets = mDevice->allocateDescriptorSets(info);

		// GATHER SHADER DATA
		const ShaderBindingData& shaderBindings = shader->GetShaderBindingsData(mTargetSet);
		const std::vector<UboData> ubosData = shaderBindings.mUbosData;

		ASSERT(shaderBindings.mSamplersData.empty());
		ASSERT(shaderBindings.mStorageImagesData.empty());
		ASSERT(shaderBindings.mStorageBuffersData.empty());

		// UNIFORM BUFFERS
		CreateUniformBuffers(ubosData);

		// UPDATE DESCRIPTOR SETS
		std::vector<vk::DescriptorBufferInfo> descriptorBuffers(ubosData.size());
		std::vector<vk::WriteDescriptorSet> descriptorWrites(ubosData.size() * IRenderer::kMaxFramesInFlight);

		for (uint32_t i = 0; i < IRenderer::kMaxFramesInFlight; ++i)
		{
			CreateUniformBuffersDescriptorInfos(descriptorBuffers, ubosData, mUniformBuffers[i]);
			CreateUniformBuffersDescriptorWrites(descriptorWrites, descriptorBuffers, ubosData, mDescriptorSets[i], i * static_cast<uint32_t>(ubosData.size()));
		}

		mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	const vk::DescriptorSet HostBindingResources::GetDescriptorSet() const
	{
		return mDescriptorSets[mCurrentBufferIndex];
	}

	void HostBindingResources::UpdateResources(vk::CommandBuffer& cmd, RenderData& data)
	{
		ASSERT(data.mRenderConfigs.IsBindingLevelCompatible(mRenderConfigs));
		for (auto& pair : mUniformBuffers[mCurrentBufferIndex])
		{
			ASSERT(data.HasUbo(pair.first));
			static_cast<HostBuffer&>(pair.second).Update(data.mUbos[pair.first].get());
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
					std::forward_as_tuple(mDevice, data.mSize));
			}
		}
	}

}