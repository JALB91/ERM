#include "erm/rendering/shaders/GPUShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/assets/models/VertexData.h>

namespace erm {

GPUShaderProgram::GPUShaderProgram(Device& device, const char* shaderPath)
	: IShaderProgram(device, shaderPath)
{}

std::vector<vk::UniqueShaderModule> GPUShaderProgram::createShaderModules(ShaderType shaderType) const
{
	const auto it = mShadersDataMap.find(shaderType);
	if (it == mShadersDataMap.end())
		return {};

	const std::vector<ShaderData>& data = it->second;

	std::vector<vk::UniqueShaderModule> result(data.size());

	for (u64 i = 0; i < data.size(); ++i)
	{
		vk::ShaderModuleCreateInfo createInfo = {};
		createInfo.codeSize = data[i].mShaderByteCode.size();
		createInfo.pCode = reinterpret_cast<const u32*>(data[i].mShaderByteCode.data());

		ERM_VK_CHECK_AND_ASSIGN(result[i], mDevice->createShaderModuleUnique(createInfo));
	}

	return result;
}

vk::VertexInputBindingDescription GPUShaderProgram::getVertexBindingDescription()
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

std::vector<vk::VertexInputAttributeDescription> GPUShaderProgram::getVertexAttributeDescriptions()
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

void GPUShaderProgram::updateBindingData()
{
	mLayoutBindingsMap.clear();
	IShaderProgram::updateBindingData();
}

void GPUShaderProgram::updateResourceBindings(
	const spirv_cross::Compiler& compiler,
	const spirv_cross::Resource& res,
	SetIdx targetSet,
	ShaderType shaderType,
	DescriptorType descriptorType)
{
	const u32 binding = compiler.get_decoration(res.id, spv::Decoration::DecorationBinding);

	for (vk::DescriptorSetLayoutBinding& layoutBinding : mLayoutBindingsMap[targetSet])
	{
		if (layoutBinding.binding == binding)
		{
			// TODO: Should find a way to assert also based on the contents of the binding
			ERM_ASSERT(layoutBinding.descriptorType == VkUtils::toVulkanValue<vk::DescriptorType>(descriptorType));
			layoutBinding.stageFlags |= VkUtils::toVulkanValue<vk::ShaderStageFlagBits>(shaderType);
			return;
		}
	}

	vk::DescriptorSetLayoutBinding& layoutBinding = mLayoutBindingsMap[targetSet].emplace_back();
	layoutBinding.binding = binding;
	layoutBinding.descriptorCount = 1;
	layoutBinding.descriptorType = VkUtils::toVulkanValue<vk::DescriptorType>(descriptorType);
	layoutBinding.stageFlags = VkUtils::toVulkanValue<vk::ShaderStageFlagBits>(shaderType);

	IShaderProgram::updateResourceBindings(
		compiler,
		res,
		targetSet,
		shaderType,
		descriptorType);
}

} // namespace erm
