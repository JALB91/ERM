#include "erm/utils/FileLocator.h"

#include <array>
#include <filesystem>
#include <map>
#include <unordered_map>

namespace {

	const char* const kObjModelExtension = ".obj";
	const char* const kColladaModelExtension = ".dae";
	const char* const kObjMaterialExtension = ".mtl";
	const char* const kVertexShaderExtension = ".vert";
	const char* const kFragmentShaderExtension = ".frag";
	const char* const kPngTextureExtension = ".png";
	const char* const kJpegTextureExtension = ".jpeg";

	const char* const kModelsDir = "models/";
	const char* const kShadersDir = "shaders/";
	const char* const kTexturesDir = "textures/";
	const char* const kMaterialsDir = "materials/";

	std::array<const char*, 2> kSupportedModelsExtensions {kObjModelExtension, kColladaModelExtension};
	std::array<const char*, 1> kSupportedMaterialsExtensions {kObjMaterialExtension};
	std::array<const char*, 1> kSupportedShadersExtensions {kVertexShaderExtension};
	std::array<const char*, 2> kSupportedTexturesExtensions {kPngTextureExtension, kJpegTextureExtension};

	std::map<std::string, std::vector<const char*>> kFilesAssociations {
		{kObjModelExtension, {kModelsDir}},
		{kColladaModelExtension, {kModelsDir}},
		{kObjMaterialExtension, {kMaterialsDir}},
		{kFragmentShaderExtension, {kShadersDir}},
		{kVertexShaderExtension, {kShadersDir}},
		{kPngTextureExtension, {kTexturesDir, kModelsDir}},
		{kJpegTextureExtension, {kTexturesDir}}};

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
					if (fileExtension.compare(extension) == 0)
					{
						files.emplace_back(includeExtension ? path : path.substr(0, path.find(extension)));
					}
				}
			}
		}

		return files;
	}

} // namespace erm
