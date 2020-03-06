#include "erm/managers/ResourcesManager.h"

#include "erm/loaders/ResourcesLoader.h"

#include "erm/utils/MeshUtils.h"
#include "erm/utils/Utils.h"

#include <algorithm>
#include <iostream>
#include <fstream>

namespace erm {
	
	ResourcesManager::ResourcesManager()
		: mResourcesLoader(std::make_unique<ResourcesLoader>())
	{}
	
	ResourcesManager::~ResourcesManager()
	{
		mResourcesLoader.reset();
	}
	
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
	
	void ResourcesManager::OnUpdate()
	{
		mResourcesLoader->OnUpdate();
	}
	
	void ResourcesManager::OnRender()
	{
		mResourcesLoader->OnRender();
	}
	
	void ResourcesManager::OnPostRender()
	{
		mResourcesLoader->OnPostRender();
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
		
		std::ifstream stream (buffer);
		if (!stream.is_open())
		{
			std::cout << "No such file: " << shaderProgramPath << std::endl;
			return nullptr;
		}
		stream.close();
		
		std::memset(buffer, 0, sizeof(buffer));
		std::strcat(buffer, shaderProgramPath);
		std::strcat(buffer, ".frag");

		stream = std::ifstream(buffer);
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
		
		std::ifstream stream (texturePath);
		
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
		
		if (mResourcesLoader->ParseModel(modelPath, mLoadedModels, mLoadedMaterials, mLoadedSkins, mLoadedAnimations))
		{
			return mLoadedModels.back().get();
		}
		
		return nullptr;
	}
	
	BonesTree* ResourcesManager::GetSkin(const char* name)
	{
		auto it = std::find_if(mLoadedSkins.begin(), mLoadedSkins.end(), [name](Handle<BonesTree>& bone) {
			return bone->GetPayload()->mName == name;
		});
		
		return (it != mLoadedSkins.end() ? (*it).get() : nullptr);
	}
	
	SkeletonAnimation* ResourcesManager::GetAnimation(const char* name)
	{
		auto it = std::find_if(mLoadedAnimations.begin(), mLoadedAnimations.end(), [name](Handle<SkeletonAnimation>& animation) {
			return animation->mName == name;
		});
		
		return (it != mLoadedAnimations.end() ? (*it).get() : nullptr);
	}
	
}
