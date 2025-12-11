#pragma once

#include "erm/rendering/shaders/IShaderProgram.h"
#include "erm/rendering/shaders/ShaderBindingData.h"

#include <vulkan/vulkan.hpp>

namespace erm {

using LayoutBindingsMap = std::unordered_map<SetIdx, std::vector<vk::DescriptorSetLayoutBinding>>;

class GPUShaderProgram : public IShaderProgram
{
public:
	GPUShaderProgram(Device& device, const char* shaderPath);

	inline const LayoutBindingsMap& getLayoutBindingsMap() const { return mLayoutBindingsMap; }

//	TODO:Damiano
	std::vector<vk::UniqueShaderModule> createShaderModules(ShaderType shaderType) const;

	vk::VertexInputBindingDescription getVertexBindingDescription();
	std::vector<vk::VertexInputAttributeDescription> getVertexAttributeDescriptions();

protected:
	// IShaderProgram
	void updateBindingData() override;
	void updateResourceBindings(
		const spirv_cross::Compiler& compiler,
		const spirv_cross::Resource& res,
		SetIdx targetSet,
		ShaderType shaderType,
		DescriptorType descriptorType) override;

	LayoutBindingsMap mLayoutBindingsMap;
};

} // namespace erm
