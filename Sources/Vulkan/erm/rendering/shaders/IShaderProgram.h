#pragma once

#include "erm/rendering/data_structs/IAsset.h"
#include "erm/rendering/data_structs/ShaderData.h"
#include "erm/rendering/data_structs/UniformBufferData.h"
#include "erm/rendering/enums/ShaderType.h"

#include <vulkan/vulkan.hpp>

#include <map>
#include <memory>
#include <vector>

namespace erm {
	class Device;
}

namespace erm {

	struct ShaderBindingData
	{
		std::vector<vk::DescriptorSetLayoutBinding> mLayoutBindings;
		std::vector<UboData> mUbosData;
		std::vector<SamplerData> mSamplersData;
		std::vector<StorageImageData> mStorageImagesData;
		std::vector<StorageBufferData> mStorageBuffersData;
#ifdef ERM_RAY_TRACING_ENABLED
		std::vector<AccelerationStructureData> mASData;
#endif
	};
	using SetIdx = uint32_t;
	using ShaderBindingsMap = std::map<SetIdx, ShaderBindingData>;

	class IShaderProgram : public IAsset
	{
	public:
		static const char* GetExtensionForShaderType(ShaderType shaderType);
		static std::string GetSuffixForShaderIndex(uint32_t index);

	public:
		IShaderProgram(Device& device, const char* shaderPath);
		virtual ~IShaderProgram() = default;

		IShaderProgram(IShaderProgram&&) = delete;
		IShaderProgram(const IShaderProgram&) = delete;

		IShaderProgram& operator=(IShaderProgram&&) = delete;
		IShaderProgram& operator=(const IShaderProgram&) = delete;

		inline bool NeedsReload() const { return mNeedsReload; }
		inline void OnReloaded() { mNeedsReload = false; }

		void SetShaderSources(const std::map<ShaderType, std::vector<std::string>>& shadersSources);

		inline const std::map<ShaderType, std::vector<ShaderData>>& GetShadersDataMap() const { return mShadersData; }
		const std::vector<ShaderData>& GetShadersData(ShaderType shaderType) const;
		std::vector<ShaderData>& GetShadersData(ShaderType shaderType);

		const ShaderBindingData& GetShaderBindingsData(SetIdx setIdx) const;
		inline const ShaderBindingsMap& GetShaderBindingsMap() const { return mShaderBindingsMap; }

		std::vector<vk::UniqueShaderModule> CreateShaderModules(ShaderType shaderType) const;

	protected:
		void CompileShadersSource(ShaderType shaderType) const;

		void UpdateShadersData(ShaderType shaderType);
		void UpdateBindingData();

		Device& mDevice;
		ShaderBindingsMap mShaderBindingsMap;
		std::map<ShaderType, std::vector<ShaderData>> mShadersData;
		bool mNeedsReload;
	};

} // namespace erm