#pragma once

#include "erm/rendering/data_structs/IAsset.h"
#include "erm/rendering/enums/DescriptorType.h"
#include "erm/rendering/enums/ShaderType.h"
#include "erm/rendering/shaders/ShaderBindingData.h"
#include "erm/rendering/shaders/ShaderData.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace erm {
	class Device;
}

namespace erm {

	using SetIdx = uint32_t;
	using ShaderBindingsMap = std::unordered_map<SetIdx, ShaderBindingData>;
	using ShadersDataMap = std::unordered_map<ShaderType, std::vector<ShaderData>>;

	class IShaderProgram : public IAsset
	{
	public:
		IShaderProgram(Device& device, const char* shaderPath);
		virtual ~IShaderProgram() = default;

		IShaderProgram(IShaderProgram&&) = delete;
		IShaderProgram(const IShaderProgram&) = delete;

		IShaderProgram& operator=(IShaderProgram&&) = delete;
		IShaderProgram& operator=(const IShaderProgram&) = delete;

		inline bool NeedsReload() const { return mNeedsReload; }
		inline void OnReloaded() { mNeedsReload = false; }

		void SetShadersSources(const std::unordered_map<ShaderType, std::vector<std::string>>& shadersSources);

		inline const ShadersDataMap& GetShadersDataMap() const { return mShadersDataMap; }
		inline const ShaderBindingsMap& GetShaderBindingsMap() const { return mShaderBindingsMap; }
		inline const ShaderBindingData& GetShaderBindingsData(SetIdx setIdx) const { return mShaderBindingsMap.at(setIdx); }

	protected:
		void CompileShadersSources(ShaderType shaderType) const;

		void UpdateShadersData(ShaderType shaderType);

		virtual void UpdateBindingData();
		void UpdateShaderBindings(
			const spirv_cross::Compiler& compiler,
			ShaderType shaderType);
		virtual void UpdateResourceBindings(
			const spirv_cross::Compiler& compiler,
			const spirv_cross::Resource& res,
			SetIdx targetSet,
			ShaderType shaderType,
			DescriptorType descriptorType);

		Device& mDevice;
		ShadersDataMap mShadersDataMap;
		ShaderBindingsMap mShaderBindingsMap;
		bool mNeedsReload;
	};

} // namespace erm
