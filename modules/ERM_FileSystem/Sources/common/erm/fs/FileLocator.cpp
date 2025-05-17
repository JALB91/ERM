#include "erm/fs/FileLocator.h"


#include <erm/log/Assert.h>

#include <erm/utils/Utils.h>

#include <array>
#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

namespace {

constexpr auto kResourcesFolderName = "res";

constexpr const char* const kWavSoundExtension = ".wav";
constexpr const char* const kMp3SoundExtension = ".mp3";
constexpr const char* const kObjModelExtension = ".obj";
constexpr const char* const kColladaModelExtension = ".dae";
constexpr const char* const kFbxModelExtension = ".fbx";
constexpr const char* const kObjMaterialExtension = ".mtl";
constexpr const char* const kVertexShaderExtension = ".vert";
#ifdef ERM_RAY_TRACING_ENABLED
constexpr const char* const kRayGenShaderExtension = ".rgen";
#endif
constexpr const char* const kPngTextureExtension = ".png";
constexpr const char* const kJpgTextureExtension = ".jpg";
constexpr const char* const kJpegTextureExtension = ".jpeg";

constexpr const char* const kMusicsDir = "sounds/musics/";
constexpr const char* const kEffectsDir = "sounds/effects/";
constexpr const char* const kModelsDir = "models/";
#if defined(ERM_VULKAN)
constexpr const char* const kShadersDir = "shaders/Vulkan/rasterization/";
#elif defined(ERM_OPENGL)
constexpr const char* const kShadersDir = "shaders/OpenGL/";
#elif defined(ERM_DX12)
constexpr const char* const kShadersDir = "shaders/DX12/";
#endif
#if defined(ERM_RAY_TRACING_ENABLED)
constexpr const char* const kRTShadersDir = "shaders/Vulkan/ray_tracing/";
#endif
constexpr const char* const kTexturesDir = "textures/";

constexpr std::array kSupportedAudioExtensions {kWavSoundExtension, kMp3SoundExtension};
constexpr std::array kSupportedModelsExtensions {kObjModelExtension, kColladaModelExtension, kFbxModelExtension};
constexpr std::array kSupportedMaterialsExtensions {kObjMaterialExtension};
constexpr std::array kSupportedShadersExtensions {kVertexShaderExtension};
#ifdef ERM_RAY_TRACING_ENABLED
constexpr std::array kSupportedRTShadersExtensions {kRayGenShaderExtension};
#endif
constexpr std::array kSupportedTexturesExtensions {kPngTextureExtension, kJpgTextureExtension, kJpegTextureExtension};

std::unordered_map<std::string, std::vector<const char*>> kFilesAssociations
{
	{kWavSoundExtension, {kMusicsDir, kEffectsDir}},
		{kMp3SoundExtension, {kMusicsDir, kEffectsDir}},
		{kObjModelExtension, {kModelsDir}},
		{kColladaModelExtension, {kModelsDir}},
		{kFbxModelExtension, {kModelsDir}},
		{kObjMaterialExtension, {kModelsDir}},
		{kVertexShaderExtension, {kShadersDir}},
#ifdef ERM_RAY_TRACING_ENABLED
		{kRayGenShaderExtension, {kRTShadersDir}},
#endif
		{kPngTextureExtension, {kTexturesDir}},
		{kJpegTextureExtension, {kTexturesDir}},
	{
		kJpgTextureExtension,
		{
			kTexturesDir
		}
	}
};

} // namespace

namespace erm {

FileLocator::FileLocator()
{
	auto path = fs::current_path();
	while (!isERMRoot(path.c_str()) && path.has_parent_path())
	{
		path = path.parent_path();
	}
	setERMRootPath(std::move(path));
}

bool FileLocator::isERMRoot(const fs::path& path) const
{
	return fs::exists(path / ".erm_root");
}

bool FileLocator::setERMRootPath(fs::path ermRootPath)
{
	if (!isERMRoot(ermRootPath))
	{
		return false;
	}
	
	mERMRootPath = std::move(ermRootPath);
	refresh();
	
	return true;
}

void FileLocator::refresh()
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
		std::vector<std::string> result = getResourcesWithExtension(ext);
		mSounds.insert(mSounds.end(), result.begin(), result.end());
	}

	for (const char* ext : kSupportedModelsExtensions)
	{
		std::vector<std::string> result = getResourcesWithExtension(ext);
		mModels.insert(mModels.end(), result.begin(), result.end());
	}

	for (const char* ext : kSupportedTexturesExtensions)
	{
		std::vector<std::string> result = getResourcesWithExtension(ext);
		mTextures.insert(mTextures.end(), result.begin(), result.end());
	}

	for (const char* ext : kSupportedMaterialsExtensions)
	{
		std::vector<std::string> result = getResourcesWithExtension(ext);
		mMaterials.insert(mMaterials.end(), result.begin(), result.end());
	}

	for (const char* ext : kSupportedShadersExtensions)
	{
		std::vector<std::string> result = getResourcesWithExtension(ext, false);
		mShaderPrograms.insert(mShaderPrograms.end(), result.begin(), result.end());
	}

#ifdef ERM_RAY_TRACING_ENABLED
	for (const char* ext : kSupportedRTShadersExtensions)
	{
		std::vector<std::string> result = getResourcesWithExtension(ext, false);
		mShaderPrograms.insert(mShaderPrograms.end(), result.begin(), result.end());
	}
#endif
}

std::vector<std::string> FileLocator::getResourcesWithExtension(const char* extension, bool includeExtension /*= true*/) const
{
	std::vector<std::string> files;

	const auto it = kFilesAssociations.find(extension);

	if (it != kFilesAssociations.end())
	{
		for (const char* dirName : it->second)
		{
			const fs::path basePath = mERMRootPath / kResourcesFolderName / dirName;
			for (auto file : fs::directory_iterator(basePath))
			{
				const auto& filePath = file.path();
				if (utils::compareNoCaseSensitive(extension, filePath.extension().string()))
				{
					const auto path = filePath.string();
					files.emplace_back(includeExtension ? path : path.substr(0, path.find(extension)));
				}
			}
		}
	}

	return files;
}

} // namespace erm
