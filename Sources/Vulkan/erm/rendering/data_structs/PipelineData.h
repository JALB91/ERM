#pragma once

#include "erm/rendering/data_structs/IBindingResources.h"
#include "erm/rendering/data_structs/BindingConfigs.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <vector>

namespace erm {

	using BindingResources = std::unique_ptr<IBindingResources>;

	class PipelineData
	{
	public:
		PipelineData(const BindingConfigs& configs)
			: mConfigs(configs)
			, mMaxSet(0)
		{}

		bool IsCompatible(const BindingConfigs& other) const
		{
			return mConfigs.IsBindingLevelCompatible(other);
		}

		void UpdateResources(vk::CommandBuffer& cmd, RenderData& renderData)
		{
			for (auto& res : mBindingResources)
				res->UpdateResources(cmd, renderData);
		}

		void PostDraw()
		{
			for (auto& res : mBindingResources)
				res->PostDraw();
		}

		std::vector<vk::DescriptorSet> GetDescriptorSets(vk::DescriptorSet& emptySet)
		{
			std::vector<vk::DescriptorSet> result(mMaxSet + 1);
			for (uint32_t i = 0; i < mMaxSet + 1; ++i)
			{
				auto it = std::find_if(mBindingResources.begin(), mBindingResources.end(), [i](const BindingResources& res) {
					return res->GetTargetSet() == i;
				});

				if (it != mBindingResources.cend())
					result[i] = (*it)->GetDescriptorSet();
				else
					result[i] = emptySet;
			}
			return result;
		}

		void AddResources(uint32_t set, BindingResources&& resources)
		{
			ASSERT(resources->GetRenderConfigs().IsBindingLevelCompatible(mConfigs));
			mMaxSet = std::max(mMaxSet, set);
			mBindingResources.emplace_back(std::move(resources));
		}

	private:
		const BindingConfigs mConfigs;
		std::vector<BindingResources> mBindingResources;
		uint32_t mMaxSet;
	};

}