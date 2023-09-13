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
	virtual ~GPUShaderProgram() = default;

	inline const LayoutBindingsMap& GetLayoutBindingsMap() const { return mLayoutBindingsMap; }

//	TODO:Damiano
	std::vector<vk::UniqueShaderModule> CreateShaderModules(ShaderType /*shaderType*/) const { return {}; }

	vk::VertexInputBindingDescription GetVertexBindingDescription() { return {}; }
	std::vector<vk::VertexInputAttributeDescription> GetVertexAttributeDescriptions() { return {}; }

protected:
	// IShaderProgram
//	void UpdateBindingData() override;
//	void UpdateResourceBindings(
//		const spirv_cross::Compiler& compiler,
//		const spirv_cross::Resource& res,
//		SetIdx targetSet,
//		ShaderType shaderType,
//		DescriptorType descriptorType) override;

	LayoutBindingsMap mLayoutBindingsMap;
};

} // namespace erm
