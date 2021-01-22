#include "erm/rendering/data_structs/BindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/Material.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

namespace {

	void CreateUniformBuffers(
		erm::Device& device,
		const std::vector<erm::UboData>& ubosData,
		erm::SwapChainsUniformBuffers& buffers)
	{
		for (const erm::UboData& data : ubosData)
		{
			for (erm::UniformBuffers& buffs : buffers)
			{
				buffs.emplace(
					std::piecewise_construct,
					std::forward_as_tuple(data.mUboId),
					std::forward_as_tuple(device, data.mSize));
			}
		}
	}

	void CreateUniformBuffersDescriptorInfos(
		std::vector<vk::DescriptorBufferInfo>& infos,
		const std::vector<erm::UboData>& ubosData,
		erm::UniformBuffers& buffers)
	{
		for (size_t i = 0; i < ubosData.size(); ++i)
		{
			const erm::UboData& data = ubosData[i];
			const erm::UniformBuffer& buffer = buffers.at(data.mUboId);

			vk::DescriptorBufferInfo& bufferInfo = infos[i];
			bufferInfo.buffer = buffer.GetBuffer();
			bufferInfo.offset = data.mOffset;
			bufferInfo.range = buffer.GetBufferSize();
		}
	}

	void CreateUniformBuffersDescriptorWrites(
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<vk::DescriptorBufferInfo>& infos,
		const std::vector<erm::UboData>& ubosData,
		vk::DescriptorSet& descriptorSet)
	{
		for (size_t i = 0; i < ubosData.size(); ++i)
		{
			const erm::UboData& data = ubosData[i];

			vk::WriteDescriptorSet& descriptorWrite = writes[i];
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = data.mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &infos[i];
		}
	}

} // namespace

namespace erm {

	BindingResources::BindingResources(
		Device& device,
		Renderer& renderer,
		const vk::DescriptorPool& descriptorPool,
		const vk::DescriptorSetLayout& descriptorSetLayout,
		const RenderConfigs& renderConfigs)
		: mDevice(device)
		, mRenderer(renderer)
		, mDescriptorPool(descriptorPool)
		, mRenderConfigs(renderConfigs)
	{
		const std::vector<vk::ImageView>& swapChainImageViews = renderer.GetSwapChainImageViews();

		// CREATE UNIFORM BUFFERS
		mUniformBuffers.resize(swapChainImageViews.size());

		const ShaderProgram* shader = mRenderConfigs.mShaderProgram;
		CreateUniformBuffers(mDevice, shader->GetUbosData(), mUniformBuffers);

		// CREATE DESCRIPTOR SETS
		const std::vector<vk::DescriptorSetLayout> layouts(swapChainImageViews.size(), descriptorSetLayout);

		vk::DescriptorSetAllocateInfo info {};
		info.setDescriptorPool(mDescriptorPool);
		info.setDescriptorSetCount(mRenderer.GetImageCount());
		info.setPSetLayouts(layouts.data());

		mDescriptorSets.resize(swapChainImageViews.size());
		mDescriptorSets = mDevice->allocateDescriptorSets(info);
	}

	BindingResources::~BindingResources()
	{
		if (!mDescriptorSets.empty())
			mDevice->freeDescriptorSets(mDescriptorPool, static_cast<uint32_t>(mDescriptorSets.size()), mDescriptorSets.data());
	}

	BindingResources::BindingResources(BindingResources&& other)
		: mDevice(other.mDevice)
		, mRenderer(other.mRenderer)
		, mDescriptorPool(other.mDescriptorPool)
		, mRenderConfigs(other.mRenderConfigs)
		, mDescriptorSets(std::move(other.mDescriptorSets))
		, mUniformBuffers(std::move(other.mUniformBuffers))
	{}

	void BindingResources::UpdateResources(RenderData& data, uint32_t index)
	{
		ASSERT(data.mRenderConfigs.IsResourcesBindingCompatible(mRenderConfigs));

		const ShaderProgram* shader = mRenderConfigs.mShaderProgram;

		const std::vector<UboData>& ubosData = shader->GetUbosData();
		std::vector<vk::DescriptorBufferInfo> descriptorBuffers(ubosData.size());

		const std::vector<SamplerData>& samplerData = shader->GetSamplerData();
		std::vector<vk::DescriptorImageInfo> imagesInfo(samplerData.size());

		std::vector<vk::WriteDescriptorSet> descriptorWrites(ubosData.size() + samplerData.size());

		CreateUniformBuffersDescriptorInfos(
			descriptorBuffers,
			ubosData,
			mUniformBuffers[index]);

		CreateUniformBuffersDescriptorWrites(
			descriptorWrites,
			descriptorBuffers,
			ubosData,
			mDescriptorSets[index]);

		for (size_t i = 0; i < samplerData.size(); ++i)
		{
			const SamplerData& sData = samplerData[i];
			Texture* texture = nullptr;

			switch (sData.mTextureType)
			{
				case TextureType::DIFFUSE:
				{
					if (mRenderConfigs.mDiffuseMap)
						texture = mRenderConfigs.mDiffuseMap;
					else if (mRenderConfigs.mMaterial->mDiffuseMap)
						texture = mRenderConfigs.mMaterial->mDiffuseMap;
					else
						texture = mRenderer.GetFallbackDiffuseMap();
					break;
				}
				case TextureType::NORMAL:
				{
					if (mRenderConfigs.mNormalMap)
						texture = mRenderConfigs.mNormalMap;
					else if (mRenderConfigs.mMaterial->mNormalMap)
						texture = mRenderConfigs.mMaterial->mNormalMap;
					else
						texture = mRenderer.GetFallbackNormalMap();
					break;
				}
				default:
				{
					ASSERT(false);
					texture = mRenderer.GetFallbackDiffuseMap();
					break;
				}
			}

			vk::DescriptorImageInfo& imageInfo = imagesInfo[i];
			imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imageInfo.imageView = texture->GetImageView();
			imageInfo.sampler = mRenderer.GetTextureSampler();

			vk::WriteDescriptorSet descriptorWrite;
			descriptorWrite.dstSet = mDescriptorSets[index];
			descriptorWrite.dstBinding = shader->GetSamplerData()[i].mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imageInfo;

			descriptorWrites[ubosData.size() + i] = std::move(descriptorWrite);
		}

		mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		for (auto& pair : mUniformBuffers[index])
		{
			pair.second.Update(data.mUbos[pair.first].get());
		}
	}

} // namespace erm
