#include "erm/rendering/shaders/ShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/VertexData.h"

namespace erm {

	ShaderProgram::ShaderProgram(Device& device, const char* shaderPath)
		: IShaderProgram(device, shaderPath)
	{
		mShadersData[ShaderType::VERTEX] = GatherShaderData(ShaderType::VERTEX);
		mShadersData[ShaderType::FRAGMENT] = GatherShaderData(ShaderType::FRAGMENT);
		UpdateBindingData();
	}

	void ShaderProgram::SetShaderSources(const std::string& vertex, const std::string& fragment)
	{
		Utils::WriteToFile((mPath + ".vert").c_str(), vertex);
		Utils::WriteToFile((mPath + ".frag").c_str(), fragment);

		CompileShaderSource(ShaderType::VERTEX);
		CompileShaderSource(ShaderType::FRAGMENT);

		mShadersData.clear();
		mShadersData[ShaderType::VERTEX] = GatherShaderData(ShaderType::VERTEX);
		mShadersData[ShaderType::FRAGMENT] = GatherShaderData(ShaderType::FRAGMENT);

		UpdateBindingData();

		mNeedsReload = true;
	}

	vk::VertexInputBindingDescription ShaderProgram::GetVertexBindingDescription()
	{
		spirv_cross::ShaderResources resources = mShadersData[ShaderType::VERTEX].mShaderCompiler->get_shader_resources();

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
		const ShaderData& data = mShadersData[ShaderType::VERTEX];
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
			else if (res.name.compare("inDebugBoneId") == 0)
			{
				description.format = vk::Format::eR32Sint;
				description.offset = offsetof(VertexData, mDebugBoneId);
			}

			attributeDescriptions.emplace_back(std::move(description));
		}

		return attributeDescriptions;
	}

} // namespace erm
