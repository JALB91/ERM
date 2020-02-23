#include "utils/FileLocator.h"

#include <array>
#include <map>
#include <filesystem>
#include <unordered_map>

namespace {
	
	const char* const kModelsDir = "models/";
	const char* const kShadersDir = "shaders/";
	const char* const kMaterialsDir = "models/";
	const char* const kTexturesDir = "textures/";
	
	std::array<const char*, 2> kSupportedModelsExtensions { ".obj", ".dae" };
	std::array<const char*, 1> kSupportedMaterialsExtensions { ".mtl" };
	std::array<const char*, 1> kSupportedShadersExtensions { ".vert" };
	std::array<const char*, 1> kSupportedTexturesExtensions { ".png" };
	
	std::map<std::string, const char*> kFilesAssociations
	{
		{ ".obj", kModelsDir },
		{ ".dae", kModelsDir },
		{ ".mtl", kMaterialsDir },
		{ ".frag", kShadersDir },
		{ ".vert", kShadersDir },
		{ ".png", kTexturesDir }
	};
	
}

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
			mModels.reserve(mModels.size() + result.size());
			mModels.insert(mModels.end(), result.begin(), result.end());
		}
		
		for (const char* ext : kSupportedTexturesExtensions)
		{
			std::vector<std::string> result = GetResourcesWithExtension(ext);
			mTextures.reserve(mTextures.size() + result.size());
			mTextures.insert(mTextures.end(), result.begin(), result.end());
		}
		
		for (const char* ext : kSupportedMaterialsExtensions)
		{
			std::vector<std::string> result = GetResourcesWithExtension(ext);
			mMaterials.reserve(mMaterials.size() + result.size());
			mMaterials.insert(mMaterials.end(), result.begin(), result.end());
		}
		
		for (const char* ext : kSupportedShadersExtensions)
		{
			std::vector<std::string> result = GetResourcesWithExtension(ext, false);
			mShaderPrograms.reserve(mShaderPrograms.size() + result.size());
			mShaderPrograms.insert(mShaderPrograms.end(), result.begin(), result.end());
		}
	}
	
	std::vector<std::string> FileLocator::GetResourcesWithExtension(const char* extension, bool includeExtension /*= true*/) const
	{
		if (mResourcesRoot.empty()) return {};
		
		std::vector<std::string> files;
		
		auto it = kFilesAssociations.find(extension);
		
		if (it != kFilesAssociations.end())
		{
			std::string basePath = mResourcesRoot + it->second;
			for (auto file : std::filesystem::directory_iterator(basePath))
			{
				std::string path = file.path().string();
				if (path.find(extension) != std::string::npos)
				{
					files.emplace_back(includeExtension ? path : path.substr(0, path.find(extension)));
				}
			}
		}
		
		return files;
	}
	
}
