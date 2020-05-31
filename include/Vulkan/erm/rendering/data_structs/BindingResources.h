#pragma once

#include "erm/rendering/buffers/UniformBuffer.h"
#include "erm/rendering/data_structs/RenderConfigs.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <vector>

namespace erm {
	class RenderingResources;
	struct RenderData;
} // namespace erm

namespace erm {

	class BindingResources
	{
	public:
		BindingResources(RenderingResources& renderingResources, const RenderConfigs& renderConfigs);
		~BindingResources();

		BindingResources(const BindingResources&) = delete;
		BindingResources& operator=(const BindingResources&) = delete;

		BindingResources(BindingResources&& other);
		BindingResources& operator=(BindingResources&&) = delete;

		void UpdateResources(RenderData& data, uint32_t index);

		inline vk::DescriptorSet* GetDescriptorSet(uint32_t index) { return &mDescriptorSets[index]; }

	private:
		RenderingResources& mRenderingResources;
		const RenderConfigs mRenderConfigs;
		std::vector<vk::DescriptorSet> mDescriptorSets;
		std::vector<UniformBuffer> mUniformBuffers;
	};

} // namespace erm
