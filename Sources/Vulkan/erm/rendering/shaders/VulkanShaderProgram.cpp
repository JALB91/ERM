#include "erm/rendering/shaders/VulkanShaderProgram.h"

#include "erm/rendering/Device.h"

#include "erm/utils/VkUtils.h"

namespace erm {

	VulkanShaderProgram::VulkanShaderProgram(Device& device, const char* shaderPath)
		: IShaderProgram(device, shaderPath)
	{}

	std::vector<vk::UniqueShaderModule> VulkanShaderProgram::CreateShaderModules(ShaderType shaderType) const
	{
		const auto it = mShadersDataMap.find(shaderType);
		if (it == mShadersDataMap.end())
			return {};

		const std::vector<ShaderData>& data = it->second;

		std::vector<vk::UniqueShaderModule> result(data.size());

		for (size_t i = 0; i < data.size(); ++i)
		{
			vk::ShaderModuleCreateInfo createInfo = {};
			createInfo.codeSize = data[i].mShaderByteCode.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(data[i].mShaderByteCode.data());

			result[i] = mDevice->createShaderModuleUnique(createInfo);
		}

		return result;
	}

	void VulkanShaderProgram::UpdateBindingData()
	{
		mLayoutBindingsMap.clear();
		IShaderProgram::UpdateBindingData();
	}

	void VulkanShaderProgram::UpdateResourceBindings(
		const spirv_cross::Compiler& compiler,
		const spirv_cross::Resource& res,
		SetIdx targetSet,
		ShaderType shaderType,
		DescriptorType descriptorType)
	{
		const uint32_t binding = compiler.get_decoration(res.id, spv::Decoration::DecorationBinding);

		for (vk::DescriptorSetLayoutBinding& layoutBinding : mLayoutBindingsMap[targetSet])
		{
			if (layoutBinding.binding == binding)
			{
				// TODO: Should find a way to assert also based on the contents of the binding
				ASSERT(layoutBinding.descriptorType == VkUtils::ToVulkanValue<vk::DescriptorType>(descriptorType));
				layoutBinding.stageFlags |= VkUtils::ToVulkanValue<vk::ShaderStageFlagBits>(shaderType);
				return;
			}
		}

		vk::DescriptorSetLayoutBinding& layoutBinding = mLayoutBindingsMap[targetSet].emplace_back();
		layoutBinding.binding = binding;
		layoutBinding.descriptorCount = 1;
		layoutBinding.descriptorType = VkUtils::ToVulkanValue<vk::DescriptorType>(descriptorType);
		layoutBinding.stageFlags = VkUtils::ToVulkanValue<vk::ShaderStageFlagBits>(shaderType);

		IShaderProgram::UpdateResourceBindings(
			compiler,
			res,
			targetSet,
			shaderType,
			descriptorType);
	}

} // namespace erm