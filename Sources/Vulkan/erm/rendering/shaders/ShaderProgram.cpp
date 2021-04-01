#include "erm/rendering/shaders/ShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/data_structs/VertexData.h"

namespace erm {

ShaderProgram::ShaderProgram(Device& device, const char* shaderPath)
	: VulkanShaderProgram(device, shaderPath)
{
	UpdateShadersData(ShaderType::VERTEX);
	UpdateShadersData(ShaderType::FRAGMENT);
	UpdateBindingData();
}

vk::VertexInputBindingDescription ShaderProgram::GetVertexBindingDescription()
{
	ASSERT(mShadersDataMap[ShaderType::VERTEX].size() == 1);

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

std::vector<vk::VertexInputAttributeDescription> ShaderProgram::GetVertexAttributeDescriptions()
{
	ASSERT(mShadersDataMap[ShaderType::VERTEX].size() == 1);

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

} // namespace erm
