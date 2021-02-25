#pragma once

#include "erm/rendering/buffers/IBuffer.h"
#include "erm/rendering/data_structs/RenderConfigs.h"
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
		inline const BindingConfigs& GetRenderConfigs() const { return mConfigs; }
		virtual const vk::DescriptorSet GetDescriptorSet() const = 0;

		virtual void UpdateResources(vk::CommandBuffer& cmd, IRenderData& data) = 0;
		virtual void PostDraw() {};

	protected:
		template<typename Buffer>
		void CreateUniformBuffersDescriptorInfos(
			std::vector<vk::DescriptorBufferInfo>& infos,
			const std::vector<UboData>& ubosData,
			const std::map<UboId, Buffer>& buffers)
		{
			for (size_t i = 0; i < ubosData.size(); ++i)
			{
				const UboData& data = ubosData[i];
				const IBuffer& buffer = buffers.at(data.mUboId);

				vk::DescriptorBufferInfo& bufferInfo = infos[i];
				bufferInfo.buffer = buffer.GetBuffer();
				bufferInfo.offset = data.mOffset;
				bufferInfo.range = buffer.GetBufferSize();
			}
		}

		void CreateUniformBuffersDescriptorWrites(
			std::vector<vk::WriteDescriptorSet>& writes,
			const std::vector<vk::DescriptorBufferInfo>& infos,
			const std::vector<UboData>& ubosData,
			vk::DescriptorSet& descriptorSet,
			uint32_t writesOffset = 0);

		Device& mDevice;
		IRenderer& mRenderer;
		const uint32_t mTargetSet;
		const vk::DescriptorPool& mDescriptorPool;
		const IShaderProgram& mShaderProgram;
		const BindingConfigs mConfigs;
		std::vector<vk::UniqueDescriptorSet> mDescriptorSets;
	};

} // namespace erm
