#include "erm/rendering/data_structs/DeviceBindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/IRenderer.h"
#include "erm/rendering/shaders/IShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

namespace erm {

	DeviceBindingResources::DeviceBindingResources(
		Device& device,
		IRenderer& renderer,
		uint32_t targetSet,
		const vk::DescriptorPool& descriptorPool,
		const IShaderProgram& shaderProgram,
		const BindingConfigs& configs,
		const vk::DescriptorSetLayout& descriptorSetLayout
#ifdef ERM_RAY_TRACING_ENABLED
		,
		const vk::AccelerationStructureKHR* as /* = nullptr*/
#endif
		)
		: IBindingResources(device, renderer, targetSet, descriptorPool, shaderProgram, configs)
	{
		// CREATE DESCRIPTOR SETS
		vk::DescriptorSetAllocateInfo info {};
		info.setDescriptorPool(mDescriptorPool);
		info.setDescriptorSetCount(1);
		info.setPSetLayouts(&descriptorSetLayout);

		mDescriptorSets = mDevice->allocateDescriptorSets(info);

		// GATHER SHADER DATA
		const ShaderBindingData& shaderBindings = mShaderProgram.GetShaderBindingsData(mTargetSet);
		const std::vector<UboData>& ubosData = shaderBindings.mUbosData;
		const std::vector<SamplerData>& samplerData = shaderBindings.mSamplersData;
		const std::vector<StorageImageData> storageImageData = shaderBindings.mStorageImagesData;
#ifdef ERM_RAY_TRACING_ENABLED
		const std::vector<AccelerationStructureData>& asData = shaderBindings.mASData;
#endif

		// CREATE UNIFORM BUFFERS
		CreateUniformBuffers(ubosData);

		// UPDATE DESCRIPTOR SETS
		std::vector<vk::DescriptorBufferInfo> descriptorBuffers(ubosData.size());
		std::vector<vk::DescriptorImageInfo> imagesInfo(samplerData.size());
		std::vector<vk::DescriptorImageInfo> storageImagesInfo(storageImageData.size());

		std::vector<vk::WriteDescriptorSet> descriptorWrites(ubosData.size() + samplerData.size() + storageImageData.size()
#ifdef ERM_RAY_TRACING_ENABLED
															 + asData.size()
#endif
		);

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
			Texture* texture = mConfigs.GetTexture(sData.mTextureType);
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

		for (size_t i = 0; i < storageImageData.size(); ++i)
		{
			vk::DescriptorImageInfo& imageInfo = storageImagesInfo[i];
			imageInfo.imageLayout = vk::ImageLayout::eGeneral;
			imageInfo.imageView = mRenderer.GetSwapChainImageViews()[mRenderer.GetCurrentImageIndex()];

			vk::WriteDescriptorSet descriptorWrite;
			descriptorWrite.dstSet = mDescriptorSets[0];
			descriptorWrite.dstBinding = storageImageData[i].mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eStorageImage;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfo;

			descriptorWrites[ubosData.size() + samplerData.size() + i] = std::move(descriptorWrite);
		}

#ifdef ERM_RAY_TRACING_ENABLED
		if (!asData.empty())
		{
			vk::WriteDescriptorSetAccelerationStructureKHR asInfo;
			asInfo.accelerationStructureCount = 1;
			asInfo.pAccelerationStructures = as;

			vk::WriteDescriptorSet& write = descriptorWrites.back();
			write.dstSet = mDescriptorSets[0];
			write.dstBinding = asData[0].mBinding;
			write.dstArrayElement = 0;
			write.descriptorCount = 1;
			write.descriptorType = vk::DescriptorType::eAccelerationStructureKHR;
			write.pNext = &asInfo;
		}
#endif

		mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	const vk::DescriptorSet DeviceBindingResources::GetDescriptorSet() const
	{
		ASSERT(!mDescriptorSets.empty());
		return mDescriptorSets[0];
	}

	void DeviceBindingResources::UpdateResources(vk::CommandBuffer& cmd, IRenderData& data)
	{
		for (const auto& [id, buffer] : mUniformBuffers)
		{
			ASSERT(data.HasUbo(id));
			buffer.Update(cmd, data.mUbos[id].get());
		}

		const auto& shaderBindingData = mShaderProgram.GetShaderBindingsData(mTargetSet);
		const auto& storageImageData = shaderBindingData.mStorageImagesData;
		if (!storageImageData.empty())
		{
			vk::DescriptorImageInfo imageInfo;
			imageInfo.imageLayout = vk::ImageLayout::eGeneral;
			imageInfo.imageView = mRenderer.GetSwapChainImageViews()[mRenderer.GetCurrentImageIndex()];

			vk::WriteDescriptorSet descriptorWrite;
			descriptorWrite.dstSet = mDescriptorSets[0];
			descriptorWrite.dstBinding = storageImageData[0].mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eStorageImage;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfo;

			mDevice->updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
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