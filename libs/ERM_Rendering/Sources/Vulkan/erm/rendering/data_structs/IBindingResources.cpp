#include "erm/rendering/data_structs/IBindingResources.h"

#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/renderer/IRenderer.h"
#include "erm/rendering/textures/GPUImage.h"

namespace erm {

IBindingResources::IBindingResources(
	Device& device,
	IRenderer& renderer,
	uint32_t targetSet,
	const IShaderProgram& shaderProgram,
	const BindingConfigs& configs,
	vk::DescriptorPool descriptorPool)
	: mDevice(device)
	, mRenderer(renderer)
	, mTargetSet(targetSet)
	, mShaderProgram(shaderProgram)
	, mConfigs(configs)
	, mDescriptorPool(descriptorPool)
{}

void IBindingResources::CreateStorageBuffersDescriptorWritesAndInfos(
	std::vector<std::vector<vk::DescriptorBufferInfo>>& infos,
	std::vector<vk::WriteDescriptorSet>& writes,
	const std::vector<StorageBufferData>& storageBuffersData,
	const StorageBuffersMap& buffersMap,
	vk::DescriptorSet descriptorSet)
{
	for (const StorageBufferData& data : storageBuffersData)
	{
		const StorageBufferResources& bufferResources = buffersMap.at(data.mType);
		std::vector<vk::DescriptorBufferInfo>& dbInfos = infos.emplace_back();

		const uint32_t maxId = bufferResources.GetMaxBufferId();

		dbInfos.resize(maxId + 1);

		for (uint32_t j = 0; j < maxId + 1; ++j)
		{
			vk::DescriptorBufferInfo bufferInfo = dbInfos[j];

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

		vk::WriteDescriptorSet descriptorWrite = writes.emplace_back();
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
	vk::DescriptorSet descriptorSet)
{
	for (const SamplerData& sData : samplerData)
	{
		GPUTexture* targetTexture = nullptr;

		if (std::holds_alternative<FrameBufferType>(sData.mType))
		{
			const FrameBufferType type = std::get<FrameBufferType>(sData.mType);
			const std::vector<GPUTexture*>& frameBuffers = mRenderer.GetTargetFrameBuffers(type);
			targetTexture = frameBuffers[std::min(frameBuffers.size() - 1, (size_t)mRenderer.GetCurrentImageIndex())];
		}
		else if (std::holds_alternative<TextureType>(sData.mType))
		{
			const TextureType type = std::get<TextureType>(sData.mType);
//			TODO: Damiano
//			targetTexture = mConfigs.GetTexture(type);
			if (targetTexture == nullptr)
				targetTexture = mRenderer.GetDefaultTexture(type);
		}
		else
		{
			ERM_ASSERT(false);
			return;
		}

		vk::DescriptorImageInfo imageInfo = infos.emplace_back();
		imageInfo.imageLayout = targetTexture->GetImageLayout();
		imageInfo.imageView = targetTexture->GetImageView();
		imageInfo.sampler = mRenderer.GetTextureSampler();

		vk::WriteDescriptorSet descriptorWrite = writes.emplace_back();
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
	vk::DescriptorSet descriptorSet)
{
	for (const StorageImageData& sData : storageImageData)
	{
		const std::vector<GPUTexture*>& frameBuffers = mRenderer.GetTargetFrameBuffers(sData.mFrameBufferType);
		const GPUTexture* targetTexture = frameBuffers[std::min(frameBuffers.size() - 1, (size_t)mRenderer.GetCurrentImageIndex())];
		
		vk::DescriptorImageInfo imageInfo = infos.emplace_back();
		imageInfo.imageLayout = targetTexture->GetImageLayout();
		imageInfo.imageView = targetTexture->GetImageView();

		vk::WriteDescriptorSet descriptorWrite = writes.emplace_back();
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
	vk::DescriptorSet descriptorSet)
{
	if (!asData.empty())
	{
		ERM_ASSERT(asData.size() == 1);
		vk::WriteDescriptorSetAccelerationStructureKHR info = infos.emplace_back();
		info.accelerationStructureCount = 1;
		info.pAccelerationStructures = as;

		vk::WriteDescriptorSet write = writes.emplace_back();
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
