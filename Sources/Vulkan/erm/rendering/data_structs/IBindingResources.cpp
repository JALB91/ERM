#include "erm/rendering/data_structs/IBindingResources.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/Material.h"
#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/RenderingResources.h"
#include "erm/rendering/renderer/IRenderer.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

namespace erm {

	IBindingResources::IBindingResources(
		Device& device,
		IRenderer& renderer,
		uint32_t targetSet,
		const vk::DescriptorPool& descriptorPool,
		const RenderConfigs& renderConfigs)
		: mDevice(device)
		, mRenderer(renderer)
		, mTargetSet(targetSet)
		, mDescriptorPool(descriptorPool)
		, mRenderConfigs(renderConfigs)
	{}

	IBindingResources::~IBindingResources()
	{
		if (!mDescriptorSets.empty())
			mDevice->freeDescriptorSets(mDescriptorPool, static_cast<uint32_t>(mDescriptorSets.size()), mDescriptorSets.data());
	}

	void IBindingResources::CreateUniformBuffersDescriptorWrites(
		std::vector<vk::WriteDescriptorSet>& writes,
		const std::vector<vk::DescriptorBufferInfo>& infos,
		const std::vector<UboData>& ubosData,
		vk::DescriptorSet& descriptorSet,
		uint32_t writesOffset /* = 0*/)
	{
		for (size_t i = 0; i < ubosData.size(); ++i)
		{
			const erm::UboData& data = ubosData[i];

			vk::WriteDescriptorSet& descriptorWrite = writes[i + writesOffset];
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = data.mBinding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &infos[i];
		}
	}

} // namespace erm
