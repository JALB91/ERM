#include "erm/rendering/data_structs/DeviceBindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/IRenderer.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

namespace erm {

	DeviceBindingResources::DeviceBindingResources(
		Device& device,
		IRenderer& renderer,
		uint32_t targetSet,
		const vk::DescriptorPool& descriptorPool,
		const RenderConfigs& renderConfigs,
		const vk::DescriptorSetLayout& descriptorSetLayout)
		: IBindingResources(device, renderer, targetSet, descriptorPool, renderConfigs)
	{
		const ShaderProgram* shader = mRenderConfigs.mShaderProgram;
		ASSERT(shader);

		// CREATE DESCRIPTOR SETS
		vk::DescriptorSetAllocateInfo info {};
		info.setDescriptorPool(mDescriptorPool);
		info.setDescriptorSetCount(1);
		info.setPSetLayouts(&descriptorSetLayout);

		mDescriptorSets = mDevice->allocateDescriptorSets(info);

		// GATHER SHADER DATA
		const ShaderBindingData& shaderBindings = shader->GetShaderBindingsData(mTargetSet);
		const std::vector<UboData> ubosData = shaderBindings.mUbosData;
		const std::vector<SamplerData> samplerData = shaderBindings.mSamplersData;

		// CREATE UNIFORM BUFFERS
		CreateUniformBuffers(ubosData);

		// UPDATE DESCRIPTOR SETS
		std::vector<vk::DescriptorBufferInfo> descriptorBuffers(ubosData.size());
		std::vector<vk::DescriptorImageInfo> imagesInfo(samplerData.size());

		std::vector<vk::WriteDescriptorSet> descriptorWrites(ubosData.size() + samplerData.size());

		CreateUniformBuffersDescriptorInfos(
			descriptorBuffers,
			ubosData,
			mUniformBuffers);

		CreateUniformBuffersDescriptorWrites(
			descriptorWrites,
			descriptorBuffers,
			ubosData,
			mDescriptorSets[0]);

		for (size_t i = 0; i < samplerData.size(); ++i)
		{
			const SamplerData& sData = samplerData[i];
			Texture* texture = mRenderConfigs.GetTexture(sData.mTextureType);
			if (!texture)
				texture = mRenderer.GetFallbackTexture(sData.mTextureType);

			vk::DescriptorImageInfo& imageInfo = imagesInfo[i];
			imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imageInfo.imageView = texture->GetImageView();
			imageInfo.sampler = mRenderer.GetTextureSampler();

			vk::WriteDescriptorSet descriptorWrite;
			descriptorWrite.dstSet = mDescriptorSets[0];
			descriptorWrite.dstBinding = sData.mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfo;

			descriptorWrites[ubosData.size() + i] = std::move(descriptorWrite);
		}

		mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	const vk::DescriptorSet DeviceBindingResources::GetDescriptorSet() const
	{
		ASSERT(!mDescriptorSets.empty());
		return mDescriptorSets[0];
	}

	void DeviceBindingResources::UpdateResources(vk::CommandBuffer& cmd, RenderData& data)
	{
		for (const auto& [id, buffer] : mUniformBuffers)
		{
			ASSERT(data.HasUbo(id));
			buffer.Update(cmd, data.mUbos[id].get());
		}
	}

	void DeviceBindingResources::CreateUniformBuffers(const std::vector<UboData>& ubosData)
	{
		for (const UboData& data : ubosData)
		{
			mUniformBuffers.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(data.mUboId),
				std::forward_as_tuple(mDevice, data.mSize));
		}
	}

} // namespace erm