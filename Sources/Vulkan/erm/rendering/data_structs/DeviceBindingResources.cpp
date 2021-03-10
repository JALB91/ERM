#include "erm/rendering/data_structs/DeviceBindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/renderer/IRenderer.h"
#include "erm/rendering/shaders/IShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

#include "erm/utils/VkUtils.h"

namespace erm {

	DeviceBindingResources::DeviceBindingResources(
		Device& device,
		IRenderer& renderer,
		uint32_t targetSet,
		const vk::DescriptorPool& descriptorPool,
		const IShaderProgram& shaderProgram,
		const BindingConfigs& configs,
		const IRenderData& renderData,
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

		mDescriptorSets = mDevice->allocateDescriptorSetsUnique(info);

		// GATHER SHADER DATA
		const ShaderBindingData& shaderBindings = mShaderProgram.GetShaderBindingsData(mTargetSet);
		const std::vector<UboData>& ubosData = shaderBindings.mUbosData;
		const std::vector<StorageBufferData>& storageBuffersData = shaderBindings.mStorageBuffersData;
		const std::vector<SamplerData>& samplerData = shaderBindings.mSamplersData;
		const std::vector<StorageImageData> storageImageData = shaderBindings.mStorageImagesData;
#ifdef ERM_RAY_TRACING_ENABLED
		const std::vector<AccelerationStructureData>& asData = shaderBindings.mASData;
#endif

		// CREATE UNIFORM BUFFERS
		CreateUniformBuffers(ubosData);

		// PREPARE RESOURCES
		std::vector<vk::DescriptorBufferInfo> uboBuffersInfos(ubosData.size());
		std::vector<vk::DescriptorImageInfo> imagesInfo(samplerData.size());
		std::vector<vk::DescriptorImageInfo> storageImagesInfo(storageImageData.size());
		std::vector<std::vector<vk::DescriptorBufferInfo>> storageBuffersInfos(storageBuffersData.size());
#ifdef ERM_RAY_TRACING_ENABLED
		std::vector<vk::WriteDescriptorSetAccelerationStructureKHR> asInfo(asData.size());
#endif

		std::vector<vk::WriteDescriptorSet> descriptorWrites(
			ubosData.size() +
			samplerData.size() +
			storageImageData.size() +
			storageBuffersData.size()
#ifdef ERM_RAY_TRACING_ENABLED
			+ asData.size()
#endif
		);

		// UNIFORM BUFFERS
		CreateUniformBuffersDescriptorWritesAndInfos(
			uboBuffersInfos,
			descriptorWrites,
			ubosData,
			mUniformBuffers,
			mDescriptorSets[0].get());

		// SAMPLERS
		CreateSamplerDescriptorWritesAndInfos(
			imagesInfo,
			descriptorWrites,
			samplerData,
			mDescriptorSets[0].get(),
			static_cast<uint32_t>(ubosData.size()));

		// STORAGE IMAGES
		CreateStorageImagesDescriptorWritesAndInfos(
			storageImagesInfo,
			descriptorWrites,
			storageImageData,
			mDescriptorSets[0].get(),
			static_cast<uint32_t>(ubosData.size() + samplerData.size()));

		// STORAGE BUFFERS
		CreateStorageBuffersDescriptorWritesAndInfos(
			storageBuffersInfos,
			descriptorWrites,
			storageBuffersData,
			renderData.mSbos,
			mDescriptorSets[0].get(),
			static_cast<uint32_t>(ubosData.size() + samplerData.size() + storageImageData.size()));

		// ACCELERATION STRUCTURES
#ifdef ERM_RAY_TRACING_ENABLED
		CreateASDescriptorWritesAndInfos(
			asInfo,
			descriptorWrites,
			asData,
			as,
			mDescriptorSets[0].get(),
			static_cast<uint32_t>(ubosData.size() + samplerData.size() + storageImageData.size() + storageBuffersData.size()));
#endif

		// UPDATE DS
		mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	const vk::DescriptorSet DeviceBindingResources::GetDescriptorSet() const
	{
		ASSERT(!mDescriptorSets.empty());
		return mDescriptorSets[0].get();
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
		if (storageImageData.empty())
			return;

		std::vector<vk::DescriptorImageInfo> storageImagesInfos(storageImageData.size());
		std::vector<vk::WriteDescriptorSet> writes(storageImageData.size());

		CreateStorageImagesDescriptorWritesAndInfos(
			storageImagesInfos,
			writes,
			storageImageData,
			mDescriptorSets[0].get());

		mDevice->updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
	}

	void DeviceBindingResources::CreateUniformBuffers(const std::vector<UboData>& ubosData)
	{
		for (const UboData& data : ubosData)
		{
			mUniformBuffers.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(data.mUboId),
				std::forward_as_tuple(mDevice, data.mStride));
		}
	}

} // namespace erm