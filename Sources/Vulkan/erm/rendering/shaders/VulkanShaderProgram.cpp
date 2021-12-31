#include "erm/rendering/shaders/VulkanShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/VertexData.h"

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

vk::VertexInputBindingDescription VulkanShaderProgram::GetVertexBindingDescription()
{
	ERM_ASSERT(mShadersDataMap[ShaderType::VERTEX].size() == 1);

	spirv_cross::ShaderResources resources = mShadersDataMap[ShaderType::VERTEX][0].mShaderCompiler->get_shader_resources();

	vk::VertexInputBindingDescription bindingDescription = {};

	if (!resources.stage_inputs.empty())
	{
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VertexData);
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;
	}

	return bindingDescription;
}

std::vector<vk::VertexInputAttributeDescription> VulkanShaderProgram::GetVertexAttributeDescriptions()
{
	ERM_ASSERT(mShadersDataMap[ShaderType::VERTEX].size() == 1);

	const ShaderData& data = mShadersDataMap[ShaderType::VERTEX][0];
	spirv_cross::ShaderResources resources = data.mShaderCompiler->get_shader_resources();

	std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

	for (const spirv_cross::Resource& res : resources.stage_inputs)
	{
		vk::VertexInputAttributeDescription description;

		description.binding = data.mShaderCompiler->get_decoration(res.id, spv::Decoration::DecorationBinding);
		description.location = data.mShaderCompiler->get_decoration(res.id, spv::Decoration::DecorationLocation);

		if (res.name.compare("inPosition") == 0)
		{
			description.format = vk::Format::eR32G32B32Sfloat;
			description.offset = offsetof(VertexData, mPositionVertex);
		}
		else if (res.name.compare("inNormal") == 0 || res.name.compare("inColor") == 0)
		{
			description.format = vk::Format::eR32G32B32Sfloat;
			description.offset = offsetof(VertexData, mNormalVertex);
		}
		else if (res.name.compare("inTexCoord") == 0)
		{
			description.format = vk::Format::eR32G32Sfloat;
			description.offset = offsetof(VertexData, mUVVertex);
		}
		else if (res.name.compare("inBoneWeights") == 0)
		{
			description.format = vk::Format::eR32G32B32A32Sfloat;
			description.offset = offsetof(VertexData, mBoneWeights);
		}
		else if (res.name.compare("inBoneIds") == 0)
		{
			description.format = vk::Format::eR32G32B32A32Sint;
			description.offset = offsetof(VertexData, mBoneIds);
		}
		else if (res.name.compare("inBoneNum") == 0)
		{
			description.format = vk::Format::eR32Sint;
			description.offset = offsetof(VertexData, mBoneNum);
		}
		else if (res.name.compare("inBoneDebugId") == 0)
		{
			description.format = vk::Format::eR32Sint;
			description.offset = offsetof(VertexData, mDebugBoneId);
		}

		attributeDescriptions.emplace_back(std::move(description));
	}

	return attributeDescriptions;
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
			ERM_ASSERT(layoutBinding.descriptorType == VkUtils::ToVulkanValue<vk::DescriptorType>(descriptorType));
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
