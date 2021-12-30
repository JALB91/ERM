#include "erm/rendering/data_structs/IBindingResources.h"

#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/renderer/IRenderer.h"
#include "erm/rendering/textures/Texture.h"

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

void IBindingResources::CreateStorageBuffersDescriptorWritesAndInfos(
	std::vector<std::vector<vk::DescriptorBufferInfo>>& infos,
	std::vector<vk::WriteDescriptorSet>& writes,
	const std::vector<StorageBufferData>& storageBuffersData,
	const StorageBuffersMap& buffersMap,
	vk::DescriptorSet& descriptorSet,
	uint32_t writesOffset /*= 0*/)
{
	for (size_t i = 0; i < storageBuffersData.size(); ++i)
	{
		const StorageBufferData& data = storageBuffersData[i];
		const StorageBufferResources& bufferResources = buffersMap.at(data.mType);
		std::vector<vk::DescriptorBufferInfo>& dbInfos = infos[i];

		const uint32_t maxId = bufferResources.GetMaxBufferId();

		dbInfos.resize(maxId + 1);

		for (uint32_t j = 0; j < maxId + 1; ++j)
		{
			vk::DescriptorBufferInfo& bufferInfo = dbInfos[j];

			if (vk::Buffer buffer = bufferResources.GetBuffer(j))
			{
				bufferInfo.buffer = buffer;
				bufferInfo.offset = data.mOffset;
				bufferInfo.range = VK_WHOLE_SIZE;
			}
			else
			{
				bufferInfo.buffer = nullptr;
				bufferInfo.offset = 0;
				bufferInfo.range = VK_WHOLE_SIZE;
			}
		}

		vk::WriteDescriptorSet& descriptorWrite = writes[i + writesOffset];
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = data.mBinding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = vk::DescriptorType::eStorageBuffer;
		descriptorWrite.descriptorCount = static_cast<uint32_t>(dbInfos.size());
		descriptorWrite.pBufferInfo = dbInfos.data();
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
			texture = mRenderer.GetDefaultTexture(sData.mTextureType);

		vk::DescriptorImageInfo& imageInfo = infos[i];
		imageInfo.imageLayout = texture->GetImageLayout();
		imageInfo.imageView = texture->GetImageView();
		imageInfo.sampler = mRenderer.GetTextureSampler();

		vk::WriteDescriptorSet& descriptorWrite = writes[i + writeOffset];
		descriptorWrite.dstSet = descriptorSet;
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
#ifdef ERM_RAY_TRACING_ENABLED
		imageInfo.imageLayout = vk::ImageLayout::eGeneral;
#else
		imageInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
#endif
		imageInfo.imageView = targetImage;

		vk::WriteDescriptorSet& descriptorWrite = writes[i + writeOffset];
		descriptorWrite.dstSet = descriptorSet;
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
		write.dstSet = descriptorSet;
		write.dstBinding = asData[0].mBinding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = vk::DescriptorType::eAccelerationStructureKHR;
		write.pNext = &info;
	}
}
#endif

} // namespace erm
