#pragma once

#include "erm/rendering/data_structs/BindingConfigs.h"
#include "erm/rendering/data_structs/IBindingResources.h"
#include "erm/rendering/renderer/IRenderer.h"

#include <erm/math/Types.h>

#include <erm/utils/assert/Assert.h>

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
		, mUntouchedFrames(0)
	{}

	PipelineData(PipelineData&& other) noexcept
		: mConfigs(other.mConfigs)
		, mBindingResources(std::move(other.mBindingResources))
		, mMaxSet(other.mMaxSet)
		, mUntouchedFrames(other.mUntouchedFrames)
	{}
	
	u32 getUntouchedFrames() const
	{
		return mUntouchedFrames;
	}

	bool isCompatible(const BindingConfigs& other) const
	{
		return mConfigs.isBindingLevelCompatible(other);
	}
	
	void refresh()
	{
		++mUntouchedFrames;
	}

	void updateResources(vk::CommandBuffer& cmd, IRenderData& renderData)
	{
		for (auto& res : mBindingResources)
		{
			res->updateResources(cmd, renderData);
		}
		
		mUntouchedFrames = 0;
	}

	std::vector<vk::DescriptorSet> getDescriptorSets(vk::DescriptorSet& emptySet)
	{
		std::vector<vk::DescriptorSet> result(mMaxSet + 1);
		for (u32 i = 0; i < mMaxSet + 1; ++i)
		{
			auto it = std::find_if(
				mBindingResources.begin(),
				mBindingResources.end(),
				[i](const BindingResources& res) {
					return res->getTargetSet() == i;
			});

			if (it != mBindingResources.cend())
			{
				result[i] = (*it)->getDescriptorSet();
			}
			else
			{
				result[i] = emptySet;
			}
		}
		return result;
	}

	void addResources(u32 set, BindingResources&& resources)
	{
		ERM_ASSERT_HARD(isCompatible(resources->getBindingConfigs()));
		mMaxSet = std::max(mMaxSet, set);
		mBindingResources.emplace_back(std::move(resources));
	}

private:
	const BindingConfigs mConfigs;
	std::vector<BindingResources> mBindingResources;
	u32 mMaxSet;
	u32 mUntouchedFrames;
};

} // namespace erm
