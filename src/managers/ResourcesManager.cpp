#include "managers/ResourcesManager.h"

#include "rendering/ShaderProgram.h"
#include "rendering/Material.h"
#include "rendering/Texture.h"
#include "rendering/Model.h"

#include "utils/ModelUtils.h"
#include "utils/Utils.h"

namespace erm {
	
	std::deque<ShaderProgram> ResourcesManager::mLoadedShaderPrograms {};
	std::deque<Material> ResourcesManager::mLoadedMaterials {};
	std::deque<Texture> ResourcesManager::mLoadedTextures {};
	std::deque<Model> ResourcesManager::mLoadedModels {};
	
	ShaderProgram* ResourcesManager::GetOrCreateShaderProgram(const char* shaderProgramPath)
	{
		auto it = std::find_if(mLoadedShaderPrograms.begin(), mLoadedShaderPrograms.end(), [shaderProgramPath](ShaderProgram& program) {
			return program.GetPath().compare(shaderProgramPath) == 0;
		});
		if (it != mLoadedShaderPrograms.end())
		{
			return &(*it);
		}
		
		if (ShaderProgram::Create(shaderProgramPath, mLoadedShaderPrograms))
		{
			return &mLoadedShaderPrograms.back();
		}
		
		return nullptr;
	}
	
	Material* ResourcesManager::GetOrCreateMaterial(const char* materialPath, const char* materialName)
	{
		auto it = std::find_if(mLoadedMaterials.begin(), mLoadedMaterials.end(), [materialPath, materialName](Material& material) {
			return (material.mPath.compare(materialPath) == 0 && material.mName.compare(materialName) == 0);
		});
		if (it != mLoadedMaterials.end())
		{
			return &(*it);
		}
		
		return nullptr;
	}
	
	Texture* ResourcesManager::GetOrCreateTexture(const char* texturePath)
	{
		auto it = std::find_if(mLoadedTextures.begin(), mLoadedTextures.end(), [texturePath](Texture& texture) {
			return texture.GetPath().compare(texturePath) == 0;
		});
		if (it != mLoadedTextures.end())
		{
			return &(*it);
		}
		
		if (Texture::Create(texturePath, mLoadedTextures))
		{
			return &mLoadedTextures.back();
		}
		
		return nullptr;
	}
	
	Model* ResourcesManager::GetOrCreateModel(const char* modelPath)
	{
		auto it = std::find_if(mLoadedModels.begin(), mLoadedModels.end(), [modelPath](Model& model) {
			return model.GetPath().compare(modelPath) == 0;
		});
		if (it != mLoadedModels.end())
		{
			return &(*it);
		}
		
		if (ModelUtils::ParseModel(modelPath, mLoadedModels, mLoadedMaterials))
		{
			return &mLoadedModels.back();
		}
		
		return nullptr;
	}
	
}
