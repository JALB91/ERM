#include "erm/rendering/shaders/IShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/shaders/ShaderUtils.h"

#include "erm/utils/Utils.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace {

std::vector<char> ReadShaderCompiled(const char* path)
{
	std::ifstream stream(path, std::ios::ate | std::ios::binary);

	if (!stream.is_open())
	{
		throw std::runtime_error("Failed to open shader file");
	}

	size_t fileSize = static_cast<size_t>(stream.tellg());
	std::vector<char> buffer(fileSize);

	stream.seekg(0);
	stream.read(buffer.data(), fileSize);

	stream.close();

	return buffer;
}

std::vector<uint32_t> LoadSpirvFile(const char* path)
{
	std::vector<char> file = ReadShaderCompiled(path);

	std::vector<uint32_t> buffer(file.size() / sizeof(uint32_t));
	memcpy(buffer.data(), file.data(), file.size());

	return buffer;
}

erm::UboData GetUboData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
{
	const auto makeUboData = [&compiler, &resource](erm::UboId id, size_t size) -> erm::UboData {
		return {
			id,
			size,
			compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset),
			compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
			compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet)};
	};

	if (resource.name.compare("UMVPOnly") == 0)
		return makeUboData(erm::UBOMVPOnly::ID, sizeof(erm::UBOMVPOnly));
	else if (resource.name.compare("UModelViewProj") == 0)
		return makeUboData(erm::UboModelViewProj::ID, sizeof(erm::UboModelViewProj));
	else if (resource.name.compare("UMaterial") == 0)
		return makeUboData(erm::UboMaterial::ID, sizeof(erm::UboMaterial));
	else if (resource.name.compare("ULight") == 0)
		return makeUboData(erm::UboLight::ID, sizeof(erm::UboLight));
	else if (resource.name.compare("UView") == 0)
		return makeUboData(erm::UboView::ID, sizeof(erm::UboView));
	else if (resource.name.compare("USkeleton") == 0)
		return makeUboData(erm::UboSkeleton::ID, sizeof(erm::UboSkeleton));
	else if (resource.name.compare("UPBMaterial") == 0)
		return makeUboData(erm::UboPBMaterial::ID, sizeof(erm::UboPBMaterial));
	else if (resource.name.compare("UPBLight") == 0)
		return makeUboData(erm::UboPBLight::ID, sizeof(erm::UboPBLight));
	else if (resource.name.compare("UBonesDebug") == 0)
		return makeUboData(erm::UboBonesDebug::ID, sizeof(erm::UboBonesDebug));
#ifdef ERM_RAY_TRACING_ENABLED
	else if (resource.name.compare("URTBasic") == 0)
		return makeUboData(erm::UboRTBasic::ID, sizeof(erm::UboRTBasic));
#endif

	ASSERT(false);

	return {erm::UBOMVPOnly::ID, sizeof(erm::UBOMVPOnly), 0, 0, 0};
}

erm::SamplerData GetSamplerData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
{
	const auto makeSamplerData = [&compiler, &resource](erm::TextureType type) -> erm::SamplerData {
		return {
			compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
			compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet),
			type};
	};

	if (resource.name.compare("diffuseSampler") == 0)
		return makeSamplerData(erm::TextureType::DIFFUSE);
	else if (resource.name.compare("normalSampler") == 0)
		return makeSamplerData(erm::TextureType::NORMAL);
	else if (resource.name.compare("specularSampler") == 0)
		return makeSamplerData(erm::TextureType::SPECULAR);
	else if (resource.name.compare("cubeMap") == 0)
		return makeSamplerData(erm::TextureType::CUBE_MAP);

	ASSERT(false);

	return {0, 0, erm::TextureType::DIFFUSE};
}

erm::StorageImageData GetStorageImageData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
{
	const auto makeStorageImageData = [&compiler, &resource](erm::StorageImageType type) -> erm::StorageImageData {
		return {
			type,
			compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
			compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet)};
	};

	if (resource.name.compare("image") == 0)
		return makeStorageImageData(erm::StorageImageType::FRAME_BUFFER);
	else if (resource.name.compare("depth") == 0)
		return makeStorageImageData(erm::StorageImageType::DEPTH_BUFFER);

	ASSERT(false);

	return {erm::StorageImageType::FRAME_BUFFER, 0, 0};
}

erm::StorageBufferData GetStorageBufferData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
{
	const auto makeStorageBufferData = [&compiler, &resource](erm::StorageBufferType type) -> erm::StorageBufferData {
		return {
			type,
			compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset),
			compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
			compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet)};
	};

	if (resource.name.compare("BVertices") == 0)
		return makeStorageBufferData(erm::StorageBufferType::VERTICES);
	else if (resource.name.compare("BIndices") == 0)
		return makeStorageBufferData(erm::StorageBufferType::INDICES);
	else if (resource.name.compare("BInstancesData") == 0)
		return makeStorageBufferData(erm::StorageBufferType::INSTANCE_DATA);

	ASSERT(false);

	return {erm::StorageBufferType::VERTICES, 0, 0, 0};
}

#ifdef ERM_RAY_TRACING_ENABLED
erm::AccelerationStructureData GetAccelerationStructureData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
{
	const auto makeAccelerationStructureData = [&compiler, &resource]() -> erm::AccelerationStructureData {
		return {
			compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
			compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet)};
	};

	if (resource.name.compare("topLevelAS") == 0)
		return makeAccelerationStructureData();

	ASSERT(false);

	return {0, 0};
}
#endif

} // namespace

namespace erm {

IShaderProgram::IShaderProgram(Device& device, const char* shaderPath)
	: IAsset(shaderPath, "")
	, mDevice(device)
	, mNeedsReload(false)
{}

void IShaderProgram::Init()
{
	for (int i = 0; i < static_cast<int>(ShaderType::COUNT); ++i)
	{
		UpdateShadersData(static_cast<ShaderType>(i));
	}

	ValidateShaders();
	UpdateBindingData();
}

void IShaderProgram::SetShadersSources(const std::unordered_map<ShaderType, std::vector<std::string>>& shadersSources)
{
	for (const auto& [type, sources] : shadersSources)
	{
		for (size_t i = 0; i < sources.size(); ++i)
		{
			Utils::WriteToFile(ShaderUtils::GetShaderFilename(mPath, static_cast<uint32_t>(i), type).c_str(), sources[i]);
		}

		CompileShadersSources(type);
		UpdateShadersData(type);
	}

	UpdateBindingData();
	mNeedsReload = true;
}

void IShaderProgram::UpdateShadersData(ShaderType shaderType)
{
	if (auto it = mShadersDataMap.find(shaderType); it != mShadersDataMap.cend())
		it->second.clear();

	size_t index = 0;

	while (true)
	{
		const std::string shaderPath = ShaderUtils::GetShaderFilename(mPath, static_cast<uint32_t>(index), shaderType);

		if (!std::filesystem::exists(shaderPath))
			break;

		++index;

		std::vector<ShaderData>& data = mShadersDataMap[shaderType];
		const std::string compiledShaderPath = shaderPath + ".cmp";
		ShaderData& d = data.emplace_back();

		d.mShaderSource = Utils::ReadFromFile(shaderPath.c_str());
		d.mShaderByteCode = ReadShaderCompiled(compiledShaderPath.c_str());
		d.mShaderCompiler = std::make_unique<spirv_cross::Compiler>(LoadSpirvFile(compiledShaderPath.c_str()));
	}
}

void IShaderProgram::CompileShadersSources(ShaderType shaderType) const
{
	ASSERT(mShadersDataMap.find(shaderType) != mShadersDataMap.end());
	const std::vector<ShaderData>& data = mShadersDataMap.at(shaderType);

	for (size_t i = 0; i < data.size(); ++i)
	{
		const std::string shaderPath = ShaderUtils::GetShaderFilename(mPath, static_cast<uint32_t>(i), shaderType);
		const std::string compiledShaderPath = shaderPath + ".cmp";
#ifdef ERM_SHADER_COMPILER
		std::string compilationCommand = ERM_SHADER_COMPILER;
		compilationCommand += " " + shaderPath + " -o " + compiledShaderPath;
		system(compilationCommand.c_str());
#else
		ASSERT(false);
#endif
	}
}

void IShaderProgram::UpdateBindingData()
{
	mShaderBindingsMap.clear();

	for (const auto& [shaderType, shadersData] : mShadersDataMap)
	{
		for (const auto& data : shadersData)
		{
			ASSERT(data.mShaderCompiler);
			UpdateShaderBindings(*data.mShaderCompiler, shaderType);
		}
	}
}

void IShaderProgram::UpdateShaderBindings(
	const spirv_cross::Compiler& compiler,
	ShaderType shaderType)
{
	const auto gatherResourceBindings = [this, &compiler, shaderType](auto& resources, DescriptorType type) {
		for (const auto& res : resources)
		{
			const uint32_t targetSet = compiler.get_decoration(res.id, spv::Decoration::DecorationDescriptorSet);
			UpdateResourceBindings(compiler, res, targetSet, shaderType, type);
		}
	};

	const spirv_cross::ShaderResources resources = compiler.get_shader_resources();

	gatherResourceBindings(resources.uniform_buffers, DescriptorType::UNIFORM_BUFFER);
	gatherResourceBindings(resources.sampled_images, DescriptorType::COMBINED_IMAGE_SAMPLER);
	gatherResourceBindings(resources.storage_images, DescriptorType::STORAGE_IMAGE);
	gatherResourceBindings(resources.storage_buffers, DescriptorType::STORAGE_BUFFER);
#ifdef ERM_RAY_TRACING_ENABLED
	gatherResourceBindings(resources.acceleration_structures, DescriptorType::ACCELERATION_STRUCTURE);
#endif
}

void IShaderProgram::UpdateResourceBindings(
	const spirv_cross::Compiler& compiler,
	const spirv_cross::Resource& res,
	SetIdx targetSet,
	ShaderType shaderType,
	DescriptorType descriptorType)
{
	UNUSED(shaderType);

	switch (descriptorType)
	{
		case DescriptorType::UNIFORM_BUFFER:
			mShaderBindingsMap[targetSet].mUbosData.emplace_back(GetUboData(compiler, res));
			break;
		case DescriptorType::COMBINED_IMAGE_SAMPLER:
			mShaderBindingsMap[targetSet].mSamplersData.emplace_back(GetSamplerData(compiler, res));
			break;
		case DescriptorType::STORAGE_IMAGE:
			mShaderBindingsMap[targetSet].mStorageImagesData.emplace_back(GetStorageImageData(compiler, res));
			break;
		case DescriptorType::STORAGE_BUFFER:
			mShaderBindingsMap[targetSet].mStorageBuffersData.emplace_back(GetStorageBufferData(compiler, res));
			break;
#ifdef ERM_RAY_TRACING_ENABLED
		case DescriptorType::ACCELERATION_STRUCTURE:
			mShaderBindingsMap[targetSet].mASData.emplace_back(GetAccelerationStructureData(compiler, res));
			break;
#endif
		default:
			ASSERT(false);
			break;
	}
}

void IShaderProgram::ValidateShaders() const
{
#if !defined(NDEBUG)
	static const std::array<ShaderType, 2> sRequiredShaderTypes {ShaderType::VERTEX, ShaderType::FRAGMENT};

	bool hasRequiredShaders = true;

	for (const ShaderType type : sRequiredShaderTypes)
		hasRequiredShaders &= mShadersDataMap.find(type) != mShadersDataMap.cend();

#	if defined(ERM_RAY_TRACING_ENABLED)
	static const std::array<ShaderType, 3> sRequiredRTShaderTypes {ShaderType::RT_RAY_GEN, ShaderType::RT_MISS, ShaderType::RT_CLOSEST_HIT};

	bool hasRequiredRTShaders = true;

	for (const ShaderType type : sRequiredRTShaderTypes)
		hasRequiredRTShaders &= mShadersDataMap.find(type) != mShadersDataMap.cend();

	ASSERT((!hasRequiredShaders && hasRequiredRTShaders) || (hasRequiredShaders && !hasRequiredRTShaders));
#	else
	ASSERT(hasRequiredShaders);
#	endif
#endif
}

} // namespace erm
