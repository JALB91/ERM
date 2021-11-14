#include "erm/utils/FileLocator.h"

#include "erm/utils/Utils.h"

#include <array>
#include <filesystem>
#include <map>

namespace {

const char* const kWavSoundExtension = ".wav";
const char* const kMp3SoundExtension = ".mp3";
const char* const kObjModelExtension = ".obj";
const char* const kColladaModelExtension = ".dae";
#if defined(ERM_FBX_ENABLED) || defined(ERM_ASSIMP_ENABLED)
const char* const kFbxModelExtension = ".fbx";
#endif
const char* const kObjMaterialExtension = ".mtl";
const char* const kVertexShaderExtension = ".vert";
#ifdef ERM_RAY_TRACING_ENABLED
const char* const kRayGenShaderExtension = ".rgen";
#endif
const char* const kPngTextureExtension = ".png";
const char* const kJpgTextureExtension = ".jpg";
const char* const kJpegTextureExtension = ".jpeg";

const char* const kMusicsDir = "sounds/musics/";
const char* const kEffectsDir = "sounds/effects/";
const char* const kModelsDir = "models/";
#if defined(ERM_VULKAN)
const char* const kShadersDir = "shaders/Vulkan/rasterization/";
#elif defined(ERM_OPENGL)
const char* const kShadersDir = "shaders/OpenGl/";
#elif defined(ERM_DX12)
const char* const kShadersDir = "shaders/DX12/";
#endif
#if defined(ERM_RAY_TRACING_ENABLED)
const char* const kRTShadersDir = "shaders/Vulkan/ray_tracing/";
#endif
const char* const kTexturesDir = "textures/";

std::array kSupportedAudioExtensions {kWavSoundExtension, kMp3SoundExtension};
std::array kSupportedModelsExtensions
{
	kObjModelExtension,
		kColladaModelExtension,
#if defined(ERM_FBX_ENABLED) || defined(ERM_ASSIMP_ENABLED)
		kFbxModelExtension
#endif
};
std::array kSupportedMaterialsExtensions {kObjMaterialExtension};
std::array kSupportedShadersExtensions {kVertexShaderExtension};
#ifdef ERM_RAY_TRACING_ENABLED
std::array kSupportedRTShadersExtensions {kRayGenShaderExtension};
#endif
std::array kSupportedTexturesExtensions {kPngTextureExtension, kJpgTextureExtension, kJpegTextureExtension};

std::map<std::string, std::vector<const char*>> kFilesAssociations
{
	{kWavSoundExtension, {kMusicsDir, kEffectsDir}},
		{kMp3SoundExtension, {kMusicsDir, kEffectsDir}},
		{kObjModelExtension, {kModelsDir}},
		{kColladaModelExtension, {kModelsDir}},
#if defined(ERM_FBX_ENABLED) || defined(ERM_ASSIMP_ENABLED)
		{kFbxModelExtension, {kModelsDir}},
#endif
		{kObjMaterialExtension, {kModelsDir}},
		{kVertexShaderExtension, {kShadersDir}},
#ifdef ERM_RAY_TRACING_ENABLED
		{kRayGenShaderExtension, {kRTShadersDir}},
#endif
		{kPngTextureExtension, {kTexturesDir}},
		{kJpegTextureExtension, {kTexturesDir}},
	{
		kJpgTextureExtension, { kTexturesDir }
	}
};

} // namespace

namespace erm {

FileLocator::FileLocator()
{
	if (std::filesystem::exists("res/"))
	{
		mResourcesRoot = "res/";
	}
	else if (std::filesystem::exists("../res/"))
	{
		mResourcesRoot = "../res/";
	}
	else if (std::filesystem::exists("../../res/"))
	{
		mResourcesRoot = "../../res/";
	}

	Refresh();
}

void FileLocator::Refresh()
{
	mSounds.clear();
	mModels.clear();
	mTextures.clear();
	mMaterials.clear();
	mShaderPrograms.clear();

	mModels.emplace_back("Defaults/Triangle");
	mModels.emplace_back("Defaults/Square");
	mModels.emplace_back("Defaults/Cube");
	mModels.emplace_back("Defaults/Sphere");
	mModels.emplace_back("Defaults/Spike");
	mModels.emplace_back("Defaults/Grid");

	for (const char* ext : kSupportedAudioExtensions)
	{
		std::vector<std::string> result = GetResourcesWithExtension(ext);
		mSounds.insert(mSounds.end(), result.begin(), result.end());
	}

	for (const char* ext : kSupportedModelsExtensions)
	{
		std::vector<std::string> result = GetResourcesWithExtension(ext);
		mModels.insert(mModels.end(), result.begin(), result.end());
	}

	for (const char* ext : kSupportedTexturesExtensions)
	{
		std::vector<std::string> result = GetResourcesWithExtension(ext);
		mTextures.insert(mTextures.end(), result.begin(), result.end());
	}

	for (const char* ext : kSupportedMaterialsExtensions)
	{
		std::vector<std::string> result = GetResourcesWithExtension(ext);
		mMaterials.insert(mMaterials.end(), result.begin(), result.end());
	}

	for (const char* ext : kSupportedShadersExtensions)
	{
		std::vector<std::string> result = GetResourcesWithExtension(ext, false);
		mShaderPrograms.insert(mShaderPrograms.end(), result.begin(), result.end());
	}

#ifdef ERM_RAY_TRACING_ENABLED
	for (const char* ext : kSupportedRTShadersExtensions)
	{
		std::vector<std::string> result = GetResourcesWithExtension(ext, false);
		mShaderPrograms.insert(mShaderPrograms.end(), result.begin(), result.end());
	}
#endif
}

std::vector<std::string> FileLocator::GetResourcesWithExtension(const char* extension, bool includeExtension /*= true*/) const
{
	if (mResourcesRoot.empty())
		return {};

	std::vector<std::string> files;

	const auto it = kFilesAssociations.find(extension);

	if (it != kFilesAssociations.end())
	{
		for (const char* dirPath : it->second)
		{
			const std::string basePath = mResourcesRoot + dirPath;
			for (auto file : std::filesystem::directory_iterator(basePath))
			{
				const std::string path = file.path().string();
				const std::string fileExtension = file.path().extension().string();
				if (Utils::CompareNoCaseSensitive(fileExtension, extension))
				{
					files.emplace_back(includeExtension ? path : path.substr(0, path.find(extension)));
				}
			}
		}
	}

	return files;
}

} // namespace erm
