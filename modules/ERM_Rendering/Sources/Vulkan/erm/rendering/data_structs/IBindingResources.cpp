#include "erm/rendering/data_structs/IBindingResources.h"

#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/textures/GPUImage.h"

namespace erm {

IBindingResources::IBindingResources(
	Device& device,
	Renderer& renderer,
	u32 targetSet,
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

void IBindingResources::createStorageBuffersDescriptorWritesAndInfos(
	std::vector<std::vector<vk::DescriptorBufferInfo>>& infos,
	std::vector<vk::WriteDescriptorSet>& writes,
	const std::vector<StorageBufferData>& storageBuffersData,
	const StorageBuffersMap& buffersMap,
	vk::DescriptorSet descriptorSet)
{
	for (const auto& data : storageBuffersData)
	{
		const auto& bufferResources = buffersMap.at(data.mType);
		auto& dbInfos = infos.emplace_back();

		const u32 maxId = bufferResources.getMaxBufferId();

		dbInfos.resize(maxId + 1);

		for (u32 j = 0; j < maxId + 1; ++j)
		{
			auto& bufferInfo = dbInfos[j];

			if (vk::Buffer buffer = bufferResources.getBuffer(j))
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

		auto& descriptorWrite = writes.emplace_back();
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = data.mBinding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = vk::DescriptorType::eStorageBuffer;
		descriptorWrite.descriptorCount = static_cast<u32>(dbInfos.size());
		descriptorWrite.pBufferInfo = dbInfos.data();
	}
}

void IBindingResources::createSamplerDescriptorWritesAndInfos(
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
			const auto type = std::get<FrameBufferType>(sData.mType);
			const auto& frameBuffers = mRenderer.getTargetFrameBuffers(type);
			const auto index = std::min(frameBuffers.size() - 1, static_cast<size_t>(mRenderer.getCurrentImageIndex()));
			targetTexture = frameBuffers[index].get();
		}
		else if (std::holds_alternative<TextureType>(sData.mType))
		{
			const auto type = std::get<TextureType>(sData.mType);
//			TODO: Damiano
//			targetTexture = mConfigs.getTexture(type);
			if (targetTexture == nullptr)
				targetTexture = mRenderer.getDefaultTexture(type);
		}
		else
		{
			ERM_LOG_ERROR("Invalid texture type");
			return;
		}

		auto& imageInfo = infos.emplace_back();
		imageInfo.imageLayout = targetTexture->getImageLayout();
		imageInfo.imageView = targetTexture->getImageView();
		imageInfo.sampler = mRenderer.getTextureSampler();

		auto& descriptorWrite = writes.emplace_back();
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = sData.mBinding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;
	}
}

void IBindingResources::createStorageImagesDescriptorWritesAndInfos(
	std::vector<vk::DescriptorImageInfo>& infos,
	std::vector<vk::WriteDescriptorSet>& writes,
	const std::vector<StorageImageData>& storageImageData,
	vk::DescriptorSet descriptorSet)
{
	for (const auto& sData : storageImageData)
	{
		const auto& frameBuffers = mRenderer.getTargetFrameBuffers(sData.mFrameBufferType);
		const auto index = std::min(frameBuffers.size() - 1, static_cast<size_t>(mRenderer.getCurrentImageIndex()));
		const auto* targetTexture = frameBuffers[index].get();
		
		auto& imageInfo = infos.emplace_back();
		imageInfo.imageLayout = targetTexture->getImageLayout();
		imageInfo.imageView = targetTexture->getImageView();

		auto& descriptorWrite = writes.emplace_back();
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = sData.mBinding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = vk::DescriptorType::eStorageImage;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;
	}
}

#ifdef ERM_RAY_TRACING_ENABLED
void IBindingResources::createASDescriptorWritesAndInfos(
	std::vector<vk::WriteDescriptorSetAccelerationStructureKHR>& infos,
	std::vector<vk::WriteDescriptorSet>& writes,
	const std::vector<AccelerationStructureData>& asData,
	const vk::AccelerationStructureKHR* as,
	vk::DescriptorSet descriptorSet)
{
	if (!asData.empty())
	{
		ERM_ASSERT(asData.size() == 1);
		auto& info = infos.emplace_back();
		info.accelerationStructureCount = 1;
		info.pAccelerationStructures = as;

		auto& write = writes.emplace_back();
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
