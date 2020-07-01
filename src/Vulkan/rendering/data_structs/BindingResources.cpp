#include "erm/rendering/data_structs/BindingResources.h"

#include "erm/rendering/Device.h"
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

			vk::DescriptorBufferInfo bufferInfo {};
			bufferInfo.buffer = buffer.GetBuffer();
			bufferInfo.offset = data.mOffset;
			bufferInfo.range = buffer.GetBufferSize();

			infos.emplace_back(bufferInfo);
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

			vk::WriteDescriptorSet descriptorWrite;
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = data.mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &infos[i];

			writes.emplace_back(descriptorWrite);
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

		ShaderProgram* shader = mRenderConfigs.mShaderProgram;
		CreateUniformBuffers(mDevice, shader->GetUbosData(), mUniformBuffers);

		// CREATE DESCRIPTOR SETS
		std::vector<vk::DescriptorSetLayout> layouts(swapChainImageViews.size(), descriptorSetLayout);

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

		ShaderProgram* shader = mRenderConfigs.mShaderProgram;
		std::vector<vk::DescriptorBufferInfo> descriptorBuffers;
		std::vector<vk::DescriptorImageInfo> descriptorImage;
		std::vector<vk::WriteDescriptorSet> descriptorWrites;

		CreateUniformBuffersDescriptorInfos(
			descriptorBuffers,
			shader->GetUbosData(),
			mUniformBuffers[index]);

		CreateUniformBuffersDescriptorWrites(
			descriptorWrites,
			descriptorBuffers,
			shader->GetUbosData(),
			mDescriptorSets[index]);

		/*
		TODO: Find a proper way to handle textures
		 */
		if (mRenderConfigs.mTexture && false)
		{
			vk::DescriptorImageInfo imageInfo {};
			imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imageInfo.imageView = mRenderConfigs.mTexture->GetImageView();
			imageInfo.sampler = mRenderer.GetTextureSampler();

			descriptorImage.emplace_back(imageInfo);
		}

		for (size_t i = 0; i < descriptorImage.size(); ++i)
		{
			vk::WriteDescriptorSet descriptorWrite;
			descriptorWrite.dstSet = mDescriptorSets[index];
			descriptorWrite.dstBinding = 1;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &descriptorImage[i];

			descriptorWrites.emplace_back(descriptorWrite);
		}

		mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		for (auto& pair : mUniformBuffers[index])
		{
			pair.second.Update(data.mUbos[pair.first].get());
		}
	}

} // namespace erm
