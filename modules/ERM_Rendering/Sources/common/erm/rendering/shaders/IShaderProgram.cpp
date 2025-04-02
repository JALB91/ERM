#include "erm/rendering/shaders/IShaderProgram.h"

#include "erm/rendering/Device.h"

#include <erm/assets/shaders/ShaderUtils.h>

#include <erm/utils/Utils.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace {

//std::vector<char> ReadShaderCompiled(std::string_view path)
//{
//	std::ifstream stream(path, std::ios::ate | std::ios::binary);
//
//	if (!ERM_EXPECT(stream.is_open(), "Could not open shader file"))
//	{
//		return {};
//	}
//
//	u64 fileSize = static_cast<u64>(stream.tellg());
//	std::vector<char> buffer(fileSize);
//
//	stream.seekg(0);
//	stream.read(buffer.data(), fileSize);
//	stream.close();
//
//	return buffer;
//}
//
//std::vector<u32> LoadSpirvFile(const std::vector<char>& compiledShader)
//{
//	std::vector<u32> buffer(compiledShader.size() / sizeof(u32));
//	memcpy(buffer.data(), compiledShader.data(), compiledShader.size());
//	return buffer;
//}
//
//erm::UboData GetUboData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
//{
//	const auto makeUboData = [&compiler, &resource](erm::UboId id, u64 size) -> erm::UboData {
//		return {
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet),
//			id,
//			size,
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset)};
//	};
//
//	if (resource.name.compare("UMVPOnly") == 0)
//		return makeUboData(erm::UboMVPOnly::ID, sizeof(erm::UboMVPOnly));
//	else if (resource.name.compare("UModelViewProj") == 0)
//		return makeUboData(erm::UboModelViewProj::ID, sizeof(erm::UboModelViewProj));
//	else if (resource.name.compare("UMaterial") == 0)
//		return makeUboData(erm::UboMaterial::ID, sizeof(erm::UboMaterial));
//	else if (resource.name.compare("ULight") == 0)
//		return makeUboData(erm::UboLight::ID, sizeof(erm::UboLight));
//	else if (resource.name.compare("UView") == 0)
//		return makeUboData(erm::UboView::ID, sizeof(erm::UboView));
//	else if (resource.name.compare("USkeleton") == 0)
//		return makeUboData(erm::UboSkeleton::ID, sizeof(erm::UboSkeleton));
//	else if (resource.name.compare("UPBMaterial") == 0)
//		return makeUboData(erm::UboPBMaterial::ID, sizeof(erm::UboPBMaterial));
//	else if (resource.name.compare("UPBLight") == 0)
//		return makeUboData(erm::UboPBLight::ID, sizeof(erm::UboPBLight));
//	else if (resource.name.compare("UBonesDebug") == 0)
//		return makeUboData(erm::UboBonesDebug::ID, sizeof(erm::UboBonesDebug));
//	else if (resource.name.compare("UCamera") == 0)
//		return makeUboData(erm::UboCamera::ID, sizeof(erm::UboCamera));
//#ifdef ERM_RAY_TRACING_ENABLED
//	else if (resource.name.compare("URTBasic") == 0)
//		return makeUboData(erm::UboRTBasic::ID, sizeof(erm::UboRTBasic));
//#endif
//
//	ERM_ASSERT(false);
//
//	return {0, 0, erm ::UboMVPOnly::ID, sizeof(erm::UboMVPOnly), 0};
//}
//
//erm::SamplerData GetSamplerData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
//{
//	const auto makeSamplerData = [&compiler, &resource](std::variant<erm::FrameBufferType, erm::TextureType> type) -> erm::SamplerData {
//		return {
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet),
//			type};
//	};
//
//	if (resource.name.compare("diffuseSampler") == 0)
//		return makeSamplerData(erm::TextureType::DIFFUSE);
//	else if (resource.name.compare("normalSampler") == 0)
//		return makeSamplerData(erm::TextureType::NORMAL);
//	else if (resource.name.compare("specularSampler") == 0)
//		return makeSamplerData(erm::TextureType::SPECULAR);
//	else if (resource.name.compare("cubeMapSampler") == 0)
//		return makeSamplerData(erm::TextureType::CUBE_MAP);
//	else if (resource.name.compare("frameSampler1") == 0)
//		return makeSamplerData(erm::FrameBufferType::FRAME_1);
//	else if (resource.name.compare("depthSampler") == 0)
//		return makeSamplerData(erm::FrameBufferType::DEPTH);
//
//	ERM_ASSERT(false);
//
//	return {0, 0, erm::TextureType::DIFFUSE};
//}
//
//erm::StorageImageData GetStorageImageData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
//{
//	const auto makeStorageImageData = [&compiler, &resource](erm::FrameBufferType type) -> erm::StorageImageData {
//		return {
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet),
//			type};
//	};
//
//	if (resource.name.compare("presentBuffer") == 0)
//		return makeStorageImageData(erm::FrameBufferType::PRESENT);
//	else if (resource.name.compare("frameBuffer1") == 0)
//		return makeStorageImageData(erm::FrameBufferType::FRAME_1);
//	else if (resource.name.compare("depthBuffer") == 0)
//		return makeStorageImageData(erm::FrameBufferType::DEPTH);
//
//	ERM_ASSERT(false);
//
//	return {0, 0, erm ::FrameBufferType::PRESENT};
//}
//
//erm::StorageBufferData GetStorageBufferData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
//{
//	const auto makeStorageBufferData = [&compiler, &resource](erm::StorageBufferType type) -> erm::StorageBufferData {
//		return {
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet),
//			type,
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationOffset)};
//	};
//
//	if (resource.name.compare("BVertices") == 0)
//		return makeStorageBufferData(erm::StorageBufferType::VERTICES);
//	else if (resource.name.compare("BIndices") == 0)
//		return makeStorageBufferData(erm::StorageBufferType::INDICES);
//	else if (resource.name.compare("BInstancesData") == 0)
//		return makeStorageBufferData(erm::StorageBufferType::INSTANCE_DATA);
//
//	ERM_ASSERT(false);
//
//	return {0, 0, erm ::StorageBufferType::VERTICES, 0};
//}
//
//#ifdef ERM_RAY_TRACING_ENABLED
//erm::AccelerationStructureData GetAccelerationStructureData(const spirv_cross::Compiler& compiler, const spirv_cross::Resource& resource)
//{
//	const auto makeAccelerationStructureData = [&compiler, &resource]() -> erm::AccelerationStructureData {
//		return {
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationBinding),
//			compiler.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet)};
//	};
//
//	if (resource.name.compare("topLevelAS") == 0)
//		return makeAccelerationStructureData();
//
//	ERM_ASSERT(false);
//
//	return {0, 0};
//}
//#endif

} // namespace

namespace erm {

//IShaderProgram::IShaderProgram(Device& device, std::string_view shaderPath)
//	: IResource(shaderPath, "")
//	, mDevice(device)
//	, mNeedsReload(false)
//{}
//
//void IShaderProgram::Init()
//{
//	for (int i = 0; i < static_cast<int>(ShaderType::COUNT); ++i)
//	{
//		UpdateShadersData(static_cast<ShaderType>(i));
//	}
//
//	ValidateShaders();
//	UpdateBindingData();
//}
//
//void IShaderProgram::SetShadersSources(const ShaderSources& shadersSources)
//{
//	for (const auto& [type, sources] : shadersSources)
//	{
//		for (u64 i = 0; i < sources.size(); ++i)
//		{
//			utils::WriteToFile(ShaderUtils::GetShaderFilename(mPath, static_cast<u32>(i), type).c_str(), sources[i]);
//		}
//
//		CompileShadersSources(type);
//		UpdateShadersData(type);
//	}
//
//	UpdateBindingData();
//	mNeedsReload = true;
//}
//
//void IShaderProgram::UpdateShadersData(ShaderType shaderType)
//{
//	if (auto it = mShadersDataMap.find(shaderType); it != mShadersDataMap.cend())
//		it->second.clear();
//
//	u64 index = 0;
//
//	while (true)
//	{
//		const std::string shaderPath = ShaderUtils::GetShaderFilename(mPath, static_cast<u32>(index), shaderType);
//
//		if (!std::filesystem::exists(shaderPath))
//			break;
//
//		++index;
//
//		std::vector<ShaderData>& data = mShadersDataMap[shaderType];
//		const std::string compiledShaderPath = shaderPath + ".cmp";
//		ShaderData& d = data.emplace_back();
//
//		d.mShaderSource = utils::ReadFromFile(shaderPath.c_str());
//		d.mShaderByteCode = ReadShaderCompiled(compiledShaderPath.c_str());
//		d.mShaderCompiler = std::make_unique<spirv_cross::Compiler>(LoadSpirvFile(d.mShaderByteCode));
//	}
//}
//
//void IShaderProgram::CompileShadersSources(ShaderType shaderType) const
//{
//	ERM_ASSERT(mShadersDataMap.find(shaderType) != mShadersDataMap.end());
//	const std::vector<ShaderData>& data = mShadersDataMap.at(shaderType);
//
//	for (u64 i = 0; i < data.size(); ++i)
//	{
//		const std::string shaderPath = ShaderUtils::GetShaderFilename(mPath, static_cast<u32>(i), shaderType);
//		const std::string compiledShaderPath = shaderPath + ".cmp";
//#ifdef ERM_SHADERS_COMPILER
//		std::string compilationCommand = ERM_SHADERS_COMPILER;
//		compilationCommand += " " + shaderPath + " -o " + compiledShaderPath;
//		system(compilationCommand.c_str());
//#else
//		ERM_ASSERT(false);
//#endif
//	}
//}
//
//void IShaderProgram::UpdateBindingData()
//{
//	mShaderBindingsMap.clear();
//
//	for (const auto& [shaderType, shadersData] : mShadersDataMap)
//	{
//		for (const auto& data : shadersData)
//		{
//			ERM_ASSERT(data.mShaderCompiler);
//			UpdateShaderBindings(*data.mShaderCompiler, shaderType);
//		}
//	}
//}
//
//void IShaderProgram::UpdateShaderBindings(
//	const spirv_cross::Compiler& compiler,
//	ShaderType shaderType)
//{
//	const auto gatherResourceBindings = [this, &compiler, shaderType](auto& resources, DescriptorType type) {
//		for (const auto& res : resources)
//		{
//			const u32 targetSet = compiler.get_decoration(res.id, spv::Decoration::DecorationDescriptorSet);
//			UpdateResourceBindings(compiler, res, targetSet, shaderType, type);
//		}
//	};
//
//	const spirv_cross::ShaderResources resources = compiler.get_shader_resources();
//
//	gatherResourceBindings(resources.uniform_buffers, DescriptorType::UNIFORM_BUFFER);
//	gatherResourceBindings(resources.sampled_images, DescriptorType::COMBINED_IMAGE_SAMPLER);
//	gatherResourceBindings(resources.storage_images, DescriptorType::STORAGE_IMAGE);
//	gatherResourceBindings(resources.storage_buffers, DescriptorType::STORAGE_BUFFER);
//#ifdef ERM_RAY_TRACING_ENABLED
//	gatherResourceBindings(resources.acceleration_structures, DescriptorType::ACCELERATION_STRUCTURE);
//#endif
//}
//
//void IShaderProgram::UpdateResourceBindings(
//	const spirv_cross::Compiler& compiler,
//	const spirv_cross::Resource& res,
//	SetIdx targetSet,
//	ShaderType shaderType,
//	DescriptorType descriptorType)
//{
//	ERM_UNUSED(shaderType);
//
//	switch (descriptorType)
//	{
//		case DescriptorType::UNIFORM_BUFFER:
//			mShaderBindingsMap[targetSet].mUbosData.emplace_back(GetUboData(compiler, res));
//			break;
//		case DescriptorType::COMBINED_IMAGE_SAMPLER:
//			mShaderBindingsMap[targetSet].mSamplersData.emplace_back(GetSamplerData(compiler, res));
//			break;
//		case DescriptorType::STORAGE_IMAGE:
//			mShaderBindingsMap[targetSet].mStorageImagesData.emplace_back(GetStorageImageData(compiler, res));
//			break;
//		case DescriptorType::STORAGE_BUFFER:
//			mShaderBindingsMap[targetSet].mStorageBuffersData.emplace_back(GetStorageBufferData(compiler, res));
//			break;
//#ifdef ERM_RAY_TRACING_ENABLED
//		case DescriptorType::ACCELERATION_STRUCTURE:
//			mShaderBindingsMap[targetSet].mASData.emplace_back(GetAccelerationStructureData(compiler, res));
//			break;
//#endif
//		default:
//			ERM_ASSERT(false);
//			break;
//	}
//}
//
//void IShaderProgram::ValidateShaders() const
//{
//#if !defined(ERM_DEBUG)
//	static constexpr std::array sRequiredShaderTypes {ShaderType::VERTEX, ShaderType::FRAGMENT};
//
//	bool hasRequiredShaders = true;
//
//	for (const ShaderType type : sRequiredShaderTypes)
//		hasRequiredShaders &= mShadersDataMap.find(type) != mShadersDataMap.cend();
//
//#	if defined(ERM_RAY_TRACING_ENABLED)
//	static constexpr std::array sRequiredRTShaderTypes {ShaderType::RT_RAY_GEN, ShaderType::RT_MISS, ShaderType::RT_CLOSEST_HIT};
//
//	bool hasRequiredRTShaders = true;
//
//	for (const ShaderType type : sRequiredRTShaderTypes)
//		hasRequiredRTShaders &= mShadersDataMap.find(type) != mShadersDataMap.cend();
//
//	ERM_ASSERT((!hasRequiredShaders && hasRequiredRTShaders) || (hasRequiredShaders && !hasRequiredRTShaders));
//#	else
//	ERM_ASSERT(hasRequiredShaders);
//#	endif
//#endif
//}

} // namespace erm
