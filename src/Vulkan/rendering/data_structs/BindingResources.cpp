#include "erm/rendering/data_structs/BindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/textures/Texture.h"

namespace erm {

	BindingResources::BindingResources(RenderingResources& renderingResources, const RenderConfigs& renderConfigs)
		: mRenderingResources(renderingResources)
		, mRenderConfigs(renderConfigs)
	{
		Device& device = mRenderingResources.mDevice;
		Renderer& renderer = mRenderingResources.mRenderer;
		const std::vector<vk::ImageView>& swapChainImageViews = renderer.GetSwapChainImageViews();

		// CREATE DESCRIPTOR SETS
		std::vector<vk::DescriptorSetLayout> layouts(swapChainImageViews.size(), mRenderingResources.mDescriptorSetLayout);

		vk::DescriptorSetAllocateInfo info {};
		info.setDescriptorPool(mRenderingResources.mDescriptorPool);
		info.setDescriptorSetCount(renderer.GetImageCount());
		info.setPSetLayouts(layouts.data());

		mDescriptorSets.resize(swapChainImageViews.size());
		mDescriptorSets = device->allocateDescriptorSets(info);

		// CREATE UNIFORM BUFFERS
		UniformBufferObject ubo {};

		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			mUniformBuffers.emplace_back(device, &ubo, sizeof(UniformBufferObject));
		}
	}

	BindingResources::~BindingResources()
	{
		Device& device = mRenderingResources.mDevice;

		if (!mDescriptorSets.empty())
			device->freeDescriptorSets(mRenderingResources.mDescriptorPool, static_cast<uint32_t>(mDescriptorSets.size()), mDescriptorSets.data());
	}

	BindingResources::BindingResources(BindingResources&& other)
		: mRenderingResources(other.mRenderingResources)
		, mRenderConfigs(other.mRenderConfigs)
		, mDescriptorSets(std::move(other.mDescriptorSets))
		, mUniformBuffers(std::move(other.mUniformBuffers))
	{}

	void BindingResources::UpdateResources(RenderData& data, uint32_t index)
	{
		Device& device = mRenderingResources.mDevice;
		Renderer& renderer = mRenderingResources.mRenderer;

		vk::DescriptorBufferInfo bufferInfo {};
		bufferInfo.buffer = mUniformBuffers[index].GetBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = mUniformBuffers[index].GetBufferSize();

		vk::DescriptorImageInfo imageInfo {};
		imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		imageInfo.imageView = mRenderConfigs.mTexture->GetImageView();
		imageInfo.sampler = renderer.GetTextureSampler();

		std::array<vk::WriteDescriptorSet, 2> descriptorWrites {};

		descriptorWrites[0].dstSet = mDescriptorSets[index];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].dstSet = mDescriptorSets[index];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		device->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		mUniformBuffers[index].Update(&data.mUBO);
	}

} // namespace erm
