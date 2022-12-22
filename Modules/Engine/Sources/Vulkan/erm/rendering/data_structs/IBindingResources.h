#pragma once

#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/BindingConfigs.h"
#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/data_structs/UniformBufferData.h"
#include "erm/rendering/data_structs/UniformBufferObject.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {
class Device;
class IRenderer;
class IShaderProgram;
struct IRenderData;
} // namespace erm

namespace erm {

class IBindingResources
{
public:
	IBindingResources(
		Device& device,
		IRenderer& renderer,
		uint32_t targetSet,
		const vk::DescriptorPool& descriptorPool,
		const IShaderProgram& shaderProgram,
		const BindingConfigs& configs);
	virtual ~IBindingResources() = default;

	IBindingResources(const IBindingResources&) = delete;
	IBindingResources& operator=(const IBindingResources&) = delete;

	IBindingResources(IBindingResources&&) = delete;
	IBindingResources& operator=(IBindingResources&&) = delete;

	inline uint32_t GetTargetSet() const { return mTargetSet; }
	inline const BindingConfigs& GetBindingConfigs() const { return mConfigs; }
	virtual const vk::DescriptorSet GetDescriptorSet() const = 0;

	virtual void UpdateResources(vk::CommandBuffer& cmd, IRenderData& data) = 0;

protected:
	template<typename Buffer>
	using UniformBuffersMap = std::map<UboId, UniformBuffer<Buffer>>;
	
protected:
	template<typename Buffer>
	void CreateUniformBuffersDescriptorWritesAndInfos(
		std::vector<vk::DescriptorBufferInfo>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<UboData>& ubosData,
		const UniformBuffersMap<Buffer>& buffers,
		vk::DescriptorSet& descriptorSet)
	{
		for (size_t i = 0; i < ubosData.size(); ++i)
		{
			const UboData& data = ubosData[i];
			const IBuffer& buffer = buffers.at(data.mUboId);

			vk::DescriptorBufferInfo& bufferInfo = infos.emplace_back();
			bufferInfo.buffer = buffer.GetBuffer();
			bufferInfo.offset = data.mOffset;
			bufferInfo.range = buffer.GetBufferSize();

			vk::WriteDescriptorSet& descriptorWrite = writes.emplace_back();
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = data.mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
		}
	}

	void CreateStorageBuffersDescriptorWritesAndInfos(
		std::vector<std::vector<vk::DescriptorBufferInfo>>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<StorageBufferData>& storageBuffersData,
		const StorageBuffersMap& buffersMap,
		vk::DescriptorSet& descriptorSet);

	void CreateSamplerDescriptorWritesAndInfos(
		std::vector<vk::DescriptorImageInfo>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<SamplerData>& samplerData,
		vk::DescriptorSet& descriptorSet);

	void CreateStorageImagesDescriptorWritesAndInfos(
		std::vector<vk::DescriptorImageInfo>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<StorageImageData>& storageImageData,
		vk::DescriptorSet& descriptorSet);

#ifdef ERM_RAY_TRACING_ENABLED
	void CreateASDescriptorWritesAndInfos(
		std::vector<vk::WriteDescriptorSetAccelerationStructureKHR>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<AccelerationStructureData>& asData,
		const vk::AccelerationStructureKHR* as,
		vk::DescriptorSet& descriptorSet);
#endif

	Device& mDevice;
	IRenderer& mRenderer;
	const uint32_t mTargetSet;
	const vk::DescriptorPool& mDescriptorPool;
	const IShaderProgram& mShaderProgram;
	const BindingConfigs mConfigs;
	std::vector<vk::UniqueDescriptorSet> mDescriptorSets;
};

} // namespace erm
