#include "erm/rendering/data_structs/IBindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/Material.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/renderer/IRenderer.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

#include "erm/utils/VkUtils.h"

namespace erm {

	IBindingResources::IBindingResources(
		Device& device,
		IRenderer& renderer,
		uint32_t targetSet,
		const vk::DescriptorPool& descriptorPool,
		const IShaderProgram& shaderProgram,
		const BindingConfigs& configs)
		: mDevice(device)
		, mRenderer(renderer)
		, mTargetSet(targetSet)
		, mDescriptorPool(descriptorPool)
		, mShaderProgram(shaderProgram)
		, mConfigs(configs)
	{}

	void IBindingResources::CreateUniformBuffersDescriptorWrites(
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<vk::DescriptorBufferInfo>& infos,
		const std::vector<UboData>& ubosData,
		vk::DescriptorSet& descriptorSet,
		uint32_t writesOffset /* = 0*/)
	{
		for (size_t i = 0; i < ubosData.size(); ++i)
		{
			const erm::UboData& data = ubosData[i];

			vk::WriteDescriptorSet& descriptorWrite = writes[i + writesOffset];
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = data.mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &infos[i];
		}
	}

	void IBindingResources::CreateSamplerDescriptorWritesAndInfos(
		std::vector<vk::DescriptorImageInfo>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<SamplerData>& samplerData,
		vk::DescriptorSet& descriptorSet,
		uint32_t writeOffset /* = 0*/)
	{
		for (size_t i = 0; i < samplerData.size(); ++i)
		{
			const SamplerData& sData = samplerData[i];
			Texture* texture = mConfigs.GetTexture(sData.mTextureType);
			if (!texture)
				texture = mRenderer.GetFallbackTexture(sData.mTextureType);

			vk::DescriptorImageInfo& imageInfo = infos[i];
			imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imageInfo.imageView = texture->GetImageView();
			imageInfo.sampler = mRenderer.GetTextureSampler();

			vk::WriteDescriptorSet& descriptorWrite = writes[i + writeOffset];
			descriptorWrite.dstSet = mDescriptorSets[0].get();
			descriptorWrite.dstBinding = sData.mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfo;
		}
	}

	void IBindingResources::CreateStorageImagesDescriptorWritesAndInfos(
		std::vector<vk::DescriptorImageInfo>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<StorageImageData>& storageImageData,
		vk::DescriptorSet& descriptorSet,
		uint32_t writeOffset /* = 0*/)
	{
		for (size_t i = 0; i < storageImageData.size(); ++i)
		{
			const StorageImageData& sData = storageImageData[i];
			vk::ImageView targetImage;

			switch (sData.mType)
			{
				case StorageImageType::FRAME_BUFFER:
					targetImage = mRenderer.GetSwapChainImageViews()[mRenderer.GetCurrentImageIndex()];
					break;
				case StorageImageType::DEPTH_BUFFER:
					targetImage = mRenderer.GetDepthImageView();
					break;
				default:
					ASSERT(false);
			}

			vk::DescriptorImageInfo& imageInfo = infos[i];
			imageInfo.imageLayout = vk::ImageLayout::eGeneral;
			imageInfo.imageView = mRenderer.GetSwapChainImageViews()[mRenderer.GetCurrentImageIndex()];

			vk::WriteDescriptorSet& descriptorWrite = writes[i + writeOffset];
			descriptorWrite.dstSet = mDescriptorSets[0].get();
			descriptorWrite.dstBinding = sData.mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eStorageImage;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfo;
		}
	}

#ifdef ERM_RAY_TRACING_ENABLED
	void IBindingResources::CreateASDescriptorWritesAndInfos(
		std::vector<vk::WriteDescriptorSetAccelerationStructureKHR>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<AccelerationStructureData>& asData,
		const vk::AccelerationStructureKHR* as,
		vk::DescriptorSet& descriptorSet,
		uint32_t writeOffset /* = 0*/)
	{
		if (!asData.empty())
		{
			ASSERT(asData.size() == 1);
			vk::WriteDescriptorSetAccelerationStructureKHR& info = infos[0];
			info.accelerationStructureCount = 1;
			info.pAccelerationStructures = as;

			vk::WriteDescriptorSet& write = writes[writeOffset];
			write.dstSet = mDescriptorSets[0].get();
			write.dstBinding = asData[0].mBinding;
			write.dstArrayElement = 0;
			write.descriptorCount = 1;
			write.descriptorType = vk::DescriptorType::eAccelerationStructureKHR;
			write.pNext = &info;
		}
	}
#endif

} // namespace erm
