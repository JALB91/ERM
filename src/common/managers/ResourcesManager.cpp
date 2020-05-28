#include "erm/managers/ResourcesManager.h"

#include "erm/loaders/ResourcesLoader.h"

#include "erm/utils/MeshUtils.h"
#include "erm/utils/Utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace erm {

	ResourcesManager::ResourcesManager(Device& device)
		: mDevice(device)
		, mResourcesLoader(std::make_unique<ResourcesLoader>(mDevice))
	{}

	ResourcesManager::~ResourcesManager()
	{
		mResourcesLoader.reset();
	}

	void ResourcesManager::LoadDefaultResources()
	{
		mLoadedModels.reserve(10);
		mLoadedModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Triangle", "Triangle"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateTriangle(mDevice));
		mLoadedModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Square", "Square"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateSquare(mDevice));
		mLoadedModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Cube", "Cube"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateCube(mDevice));
		mLoadedModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Sphere", "Sphere"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateSphere(mDevice));
		mLoadedModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Spike", "Spike"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateSpike(mDevice));
		mLoadedModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Grid", "Grid"));
		mLoadedModels.back()->AddMesh(MeshUtils::CreateGrid(mDevice));
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
		UNUSED(vertexShader);
		UNUSED(fragmentShader);
		//		std::unique_ptr<ShaderProgram> shaderProgram = std::make_unique<ShaderProgram>(mDevice, vertexShader, fragmentShader);
		//		return mLoadedShaderPrograms.emplace_back(std::move(shaderProgram)).get();
		return nullptr;
	}

	ShaderProgram* ResourcesManager::GetOrCreateShaderProgram(const char* shaderProgramPath)
	{
		auto it = std::find_if(
			mLoadedShaderPrograms.begin(),
			mLoadedShaderPrograms.end(),
			[shaderProgramPath](Handle<ShaderProgram>& program) {
				return program->GetPath().compare(shaderProgramPath) == 0;
			});

		if (it != mLoadedShaderPrograms.end())
		{
			return (*it).get();
		}

		char buffer[256] {0};
		std::strcat(buffer, shaderProgramPath);
		std::strcat(buffer, ".vert");

		std::ifstream stream(buffer);
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

		std::unique_ptr<ShaderProgram> shaderProgram = std::make_unique<ShaderProgram>(mDevice, shaderProgramPath);
		return mLoadedShaderPrograms.emplace_back(std::move(shaderProgram)).get();
	}

	Material* ResourcesManager::GetOrCreateMaterial(const char* materialPath, const char* materialName)
	{
		auto it = std::find_if(
			mLoadedMaterials.begin(),
			mLoadedMaterials.end(),
			[materialPath, materialName](Handle<Material>& material) {
				return (material->mPath.compare(materialPath) == 0 && material->mName.compare(materialName) == 0);
			});

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
			});

		if (it != mLoadedTextures.end())
		{
			return (*it).get();
		}

		std::ifstream stream(texturePath);

		if (!stream.is_open())
		{
			std::cout << "No such file: " << texturePath << std::endl;
			return nullptr;
		}

		stream.close();

		std::unique_ptr<Texture> texture = std::make_unique<Texture>(mDevice, texturePath);
		return mLoadedTextures.emplace_back(std::move(texture)).get();
	}

	Model* ResourcesManager::GetOrCreateModel(const char* modelPath)
	{
		auto it = std::find_if(
			mLoadedModels.begin(),
			mLoadedModels.end(),
			[modelPath](Handle<Model>& model) {
				return model->GetPath().compare(modelPath) == 0;
			});

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

} // namespace erm
