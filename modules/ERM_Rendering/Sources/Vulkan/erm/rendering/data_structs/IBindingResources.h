#pragma once

#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/BindingConfigs.h"
#include "erm/rendering/data_structs/IRenderData.h"
#include "erm/rendering/data_structs/UniformBufferData.h"
#include "erm/rendering/data_structs/UniformBufferObject.h"

#include <erm/math/Types.h>

#include <vulkan/vulkan.hpp>

#include <vector>

namespace erm {
class Device;
class Renderer;
class IShaderProgram;
struct IRenderData;
} // namespace erm

namespace erm {

class IBindingResources
{
public:
	IBindingResources(
		Device& device,
		Renderer& renderer,
		u32 targetSet,
		const IShaderProgram& shaderProgram,
		const BindingConfigs& configs,
		vk::DescriptorPool descriptorPool);
	virtual ~IBindingResources() = default;

	IBindingResources(const IBindingResources&) = delete;
	IBindingResources& operator=(const IBindingResources&) = delete;

	IBindingResources(IBindingResources&&) = delete;
	IBindingResources& operator=(IBindingResources&&) = delete;

	inline u32 getTargetSet() const { return mTargetSet; }
	inline const BindingConfigs& getBindingConfigs() const { return mConfigs; }
	virtual vk::DescriptorSet getDescriptorSet() const = 0;

	virtual void updateResources(vk::CommandBuffer cmd, IRenderData& data) = 0;

protected:
	template<typename Buffer>
	using UniformBuffersMap = std::map<UboId, UniformBuffer<Buffer>>;
	
protected:
	template<typename Buffer>
	void createUniformBuffersDescriptorWritesAndInfos(
		std::vector<vk::DescriptorBufferInfo>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<UboData>& ubosData,
		const UniformBuffersMap<Buffer>& buffers,
		vk::DescriptorSet descriptorSet)
	{
		for (u64 i = 0; i < ubosData.size(); ++i)
		{
			const UboData& data = ubosData[i];
			const IBuffer& buffer = buffers.at(data.mUboId);

			vk::DescriptorBufferInfo bufferInfo = infos.emplace_back();
			bufferInfo.buffer = buffer.getBuffer();
			bufferInfo.offset = data.mOffset;
			bufferInfo.range = buffer.getBufferSize();

			vk::WriteDescriptorSet descriptorWrite = writes.emplace_back();
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = data.mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
		}
	}

	void createStorageBuffersDescriptorWritesAndInfos(
		std::vector<std::vector<vk::DescriptorBufferInfo>>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<StorageBufferData>& storageBuffersData,
		const StorageBuffersMap& buffersMap,
		vk::DescriptorSet descriptorSet);

	void createSamplerDescriptorWritesAndInfos(
		std::vector<vk::DescriptorImageInfo>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<SamplerData>& samplerData,
		vk::DescriptorSet descriptorSet);

	void createStorageImagesDescriptorWritesAndInfos(
		std::vector<vk::DescriptorImageInfo>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<StorageImageData>& storageImageData,
		vk::DescriptorSet descriptorSet);

#ifdef ERM_RAY_TRACING_ENABLED
	void createASDescriptorWritesAndInfos(
		std::vector<vk::WriteDescriptorSetAccelerationStructureKHR>& infos,
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<AccelerationStructureData>& asData,
		const vk::AccelerationStructureKHR* as,
		vk::DescriptorSet descriptorSet);
#endif

	Device& mDevice;
	Renderer& mRenderer;
	const u32 mTargetSet;
	const IShaderProgram& mShaderProgram;
	const BindingConfigs mConfigs;
	vk::DescriptorPool mDescriptorPool;
	std::vector<vk::UniqueDescriptorSet> mDescriptorSets;
};

} // namespace erm
