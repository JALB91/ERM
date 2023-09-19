#pragma once

#include "erm/rendering/enums/DescriptorType.h"
#include "erm/rendering/shaders/ShaderBindingData.h"
#include "erm/rendering/shaders/ShaderData.h"

#include <erm/assets/enums/ShaderType.h>

#include <erm/math/Types.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace erm {
class Device;
}

namespace erm {

using SetIdx = u32;
using ShaderBindingsMap = std::unordered_map<SetIdx, ShaderBindingData>;
using ShadersDataMap = std::unordered_map<ShaderType, std::vector<ShaderData>>;
using ShaderSources = std::unordered_map<ShaderType, std::vector<std::string>>;

class IShaderProgram
{
//public:
//	IShaderProgram(Device& device, std::string_view shaderPath);
//	virtual ~IShaderProgram() = default;
//
//	IShaderProgram(IShaderProgram&&) = delete;
//	IShaderProgram(const IShaderProgram&) = delete;
//
//	IShaderProgram& operator=(IShaderProgram&&) = delete;
//	IShaderProgram& operator=(const IShaderProgram&) = delete;
//
//	inline bool NeedsReload() const { return mNeedsReload; }
//	inline void OnReloaded() { mNeedsReload = false; }
//
//	void Init();
//
//	void SetShadersSources(const ShaderSources& shadersSources);
//
//	inline const ShadersDataMap& getShadersDataMap() const { return mShadersDataMap; }
//	inline const ShaderBindingsMap& getShaderBindingsMap() const { return mShaderBindingsMap; }
//	inline const ShaderBindingData& getShaderBindingsData(SetIdx setIdx) const { return mShaderBindingsMap.at(setIdx); }
//
//protected:
//	void CompileShadersSources(ShaderType shaderType) const;
//
//	void UpdateShadersData(ShaderType shaderType);
//
//	virtual void UpdateBindingData();
//	void UpdateShaderBindings(
//		const spirv_cross::Compiler& compiler,
//		ShaderType shaderType);
//	virtual void UpdateResourceBindings(
//		const spirv_cross::Compiler& compiler,
//		const spirv_cross::Resource& res,
//		SetIdx targetSet,
//		ShaderType shaderType,
//		DescriptorType descriptorType);
//
//	void ValidateShaders() const;
//
//	Device& mDevice;
//	ShadersDataMap mShadersDataMap;
//	ShaderBindingsMap mShaderBindingsMap;
//	bool mNeedsReload;
};

} // namespace erm
