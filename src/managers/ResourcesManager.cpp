#include "managers/ResourcesManager.h"

#include "utils/ModelUtils.h"
#include "utils/MeshUtils.h"
#include "utils/Utils.h"

#include <algorithm>
#include <iostream>
#include <fstream>

namespace erm {
	
	ResourcesManager::ResourcesManager()
	{}
	
	ResourcesManager::~ResourcesManager()
	{}
	
	void ResourcesManager::LoadDefaultResources()
	{
		mLoadedModels.reserve(10);
		mLoadedModels.emplace_back(std::make_unique<Model>("Defaults/Triangle", "Triangle"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateTriangle());
		mLoadedModels.emplace_back(std::make_unique<Model>("Defaults/Square", "Square"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateSquare());
		mLoadedModels.emplace_back(std::make_unique<Model>("Defaults/Cube", "Cube"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateCube());
		mLoadedModels.emplace_back(std::make_unique<Model>("Defaults/Sphere", "Sphere"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateSphere());
		mLoadedModels.emplace_back(std::make_unique<Model>("Defaults/Spike", "Spike"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateSpike());
		mLoadedModels.emplace_back(std::make_unique<Model>("Defaults/Grid", "Grid"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateGrid());
	}
	
	ShaderProgram* ResourcesManager::GetOrCreateShaderProgram(const char* vertexShader, const char* fragmentShader)
	{
		return mLoadedShaderPrograms.emplace_back(
			std::make_unique<ShaderProgram>(vertexShader, fragmentShader)
		).get();
	}
	
	ShaderProgram* ResourcesManager::GetOrCreateShaderProgram(const char* shaderProgramPath)
	{
		auto it = std::find_if(
			mLoadedShaderPrograms.begin(),
			mLoadedShaderPrograms.end(),
			[shaderProgramPath](Handle<ShaderProgram>& program) {
				return program->GetPath().compare(shaderProgramPath) == 0;
			}
		);
		if (it != mLoadedShaderPrograms.end())
		{
			return (*it).get();
		}
		
		char buffer[256] {0};
		std::strcat(buffer, shaderProgramPath);
		std::strcat(buffer, ".vert");
		
		std::ifstream stream (Utils::GetRelativePath(buffer));
		if (!stream.is_open())
		{
			std::cout << "No such file: " << shaderProgramPath << std::endl;
			return nullptr;
		}
		stream.close();
		
		std::memset(buffer, 0, sizeof(buffer));
		std::strcat(buffer, shaderProgramPath);
		std::strcat(buffer, ".frag");

		stream = std::ifstream(Utils::GetRelativePath(buffer));
		if (!stream.is_open())
		{
			std::cout << "No such file: " << shaderProgramPath << std::endl;
			return nullptr;
		}
		stream.close();
		
		return mLoadedShaderPrograms.emplace_back(
			std::make_unique<ShaderProgram>(shaderProgramPath)
		).get();
	}
	
	Material* ResourcesManager::GetOrCreateMaterial(const char* materialPath, const char* materialName)
	{
		auto it = std::find_if(
			mLoadedMaterials.begin(),
			mLoadedMaterials.end(),
			[materialPath, materialName](Handle<Material>& material) {
				return (material->mPath.compare(materialPath) == 0 && material->mName.compare(materialName) == 0);
			}
		);
		if (it != mLoadedMaterials.end())
		{
			return (*it).get();
		}
		
		return nullptr;
	}
	
	Texture* ResourcesManager::GetOrCreateTexture(const char* texturePath)
	{
		auto it = std::find_if(
			mLoadedTextures.begin(),
			mLoadedTextures.end(),
			[texturePath](Handle<Texture>& texture) {
				return texture->GetPath().compare(texturePath) == 0;
			}
		);
		if (it != mLoadedTextures.end())
		{
			return (*it).get();
		}
		
		std::ifstream stream (Utils::GetRelativePath(texturePath));
		
		if (!stream.is_open())
		{
			std::cout << "No such file: " << texturePath << std::endl;
			return nullptr;
		}

		stream.close();
		
		return mLoadedTextures.emplace_back(
			std::make_unique<Texture>(texturePath)
		).get();
	}
	
	Model* ResourcesManager::GetOrCreateModel(const char* modelPath)
	{
		auto it = std::find_if(
			mLoadedModels.begin(),
			mLoadedModels.end(),
			[modelPath](Handle<Model>& model) {
				return model->GetPath().compare(modelPath) == 0;
			}
		);
		if (it != mLoadedModels.end())
		{
			return (*it).get();
		}
		
		if (ModelUtils::ParseModel(modelPath, mLoadedModels, mLoadedMaterials))
		{
			return mLoadedModels.back().get();
		}
		
		return nullptr;
	}
	
}
