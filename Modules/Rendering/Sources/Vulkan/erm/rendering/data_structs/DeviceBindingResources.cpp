#include "erm/rendering/data_structs/DeviceBindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/shaders/IShaderProgram.h"
#include "erm/rendering/textures/GPUImage.h"
#include "erm/rendering/utils/VkUtils.h"

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

	ERM_VK_CHECK_AND_ASSIGN(mDescriptorSets, mDevice->allocateDescriptorSetsUnique(info));

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
	std::vector<vk::DescriptorBufferInfo> uboBuffersInfos;
	uboBuffersInfos.reserve(ubosData.size());
	
	std::vector<vk::DescriptorImageInfo> imagesInfo;
	imagesInfo.reserve(samplerData.size());
	
	std::vector<vk::DescriptorImageInfo> storageImagesInfo;
	storageImagesInfo.reserve(storageImageData.size());
	
	std::vector<std::vector<vk::DescriptorBufferInfo>> storageBuffersInfos;
	storageBuffersInfos.reserve(storageBuffersData.size());
	
#ifdef ERM_RAY_TRACING_ENABLED
	std::vector<vk::WriteDescriptorSetAccelerationStructureKHR> asInfo;
	asInfo.reserve(asData.size());
#endif

	std::vector<vk::WriteDescriptorSet> descriptorWrites;
	descriptorWrites.reserve(
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
		mUniformBuffersMap,
		mDescriptorSets[0].get());

	// SAMPLERS
	CreateSamplerDescriptorWritesAndInfos(
		imagesInfo,
		descriptorWrites,
		samplerData,
		mDescriptorSets[0].get());

	// STORAGE IMAGES
	CreateStorageImagesDescriptorWritesAndInfos(
		storageImagesInfo,
		descriptorWrites,
		storageImageData,
		mDescriptorSets[0].get());

	// STORAGE BUFFERS
	CreateStorageBuffersDescriptorWritesAndInfos(
		storageBuffersInfos,
		descriptorWrites,
		storageBuffersData,
		renderData.mSbos,
		mDescriptorSets[0].get());

	// ACCELERATION STRUCTURES
#ifdef ERM_RAY_TRACING_ENABLED
	CreateASDescriptorWritesAndInfos(
		asInfo,
		descriptorWrites,
		asData,
		as,
		mDescriptorSets[0].get());
#endif

	// UPDATE DS
	mDevice->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

const vk::DescriptorSet DeviceBindingResources::GetDescriptorSet() const
{
	ERM_ASSERT(!mDescriptorSets.empty());
	return mDescriptorSets[0].get();
}

void DeviceBindingResources::UpdateResources(vk::CommandBuffer& cmd, IRenderData& data)
{
	for (const auto& [id, buffer] : mUniformBuffersMap)
	{
		ERM_ASSERT(data.HasUbo(id));
		buffer.Update(cmd, data.mUbos[id].get());
	}
}

void DeviceBindingResources::CreateUniformBuffers(const std::vector<UboData>& ubosData)
{
	for (const UboData& data : ubosData)
	{
		mUniformBuffersMap.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(data.mUboId),
			std::forward_as_tuple(mDevice, data.mStride));
	}
}

} // namespace erm
