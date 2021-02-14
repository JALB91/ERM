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

	class IShaderProgram : public IAsset
	{
	public:
		IShaderProgram(Device& device, const char* shaderPath);
		virtual ~IShaderProgram() = default;

		IShaderProgram(IShaderProgram&&) = delete;
		IShaderProgram(const IShaderProgram&) = delete;

		IShaderProgram& operator=(IShaderProgram&&) = delete;
		IShaderProgram& operator=(const IShaderProgram&) = delete;

		inline const std::vector<UboData>& GetUbosData() const { return mUbosData; }
		inline const std::vector<SamplerData>& GetSamplerData() const { return mSamplerData; }
		inline const std::vector<StorageImageData>& GetStorageImageData() const { return mStorageImageData; }

		inline bool NeedsReload() const { return mNeedsReload; }
		inline void OnReloaded() { mNeedsReload = false; }

		const ShaderData& GetShaderData(ShaderType shaderType) const;
		ShaderData& GetShaderData(ShaderType shaderType);

		vk::ShaderModule CreateShaderModule(ShaderType shaderType) const;
		std::vector<vk::DescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings() const;

	protected:
		void CompileShaderSource(ShaderType shaderType) const;

		void UpdateShaderData(ShaderType shaderType);
		void UpdateBindingData();

		Device& mDevice;
		std::vector<UboData> mUbosData;
		std::vector<SamplerData> mSamplerData;
		std::vector<StorageImageData> mStorageImageData;
		std::map<ShaderType, ShaderData> mShadersData;
		bool mNeedsReload;
	};

} // namespace erm