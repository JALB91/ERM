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
		mModels.reserve(10);
		mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Triangle", "Triangle"));
		mModels.back()->AddMesh(MeshUtils::CreateTriangle(mDevice));
		mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Square", "Square"));
		mModels.back()->AddMesh(MeshUtils::CreateSquare(mDevice));
		mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Cube", "Cube"));
		mModels.back()->AddMesh(MeshUtils::CreateCube(mDevice));
		mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Sphere", "Sphere"));
		mModels.back()->AddMesh(MeshUtils::CreateSphere(mDevice));
		mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Spike", "Spike"));
		mModels.back()->AddMesh(MeshUtils::CreateSpike(mDevice));
		mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Grid", "Grid"));
		mModels.back()->AddMesh(MeshUtils::CreateGrid(mDevice));
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

	ShaderProgram* ResourcesManager::GetOrCreateShaderProgram(const char* shaderProgramPath)
	{
		auto it = std::find_if(
			mShaderPrograms.begin(),
			mShaderPrograms.end(),
			[shaderProgramPath](Handle<ShaderProgram>& program) {
				return program->mPath.compare(shaderProgramPath) == 0;
			});

		if (it != mShaderPrograms.end())
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
		return mShaderPrograms.emplace_back(std::move(shaderProgram)).get();
	}

	Material* ResourcesManager::GetOrCreateMaterial(const char* materialPath, const char* materialName)
	{
		auto it = std::find_if(
			mMaterials.begin(),
			mMaterials.end(),
			[materialPath, materialName](Handle<Material>& material) {
				return (material->mPath.compare(materialPath) == 0 && material->mName.compare(materialName) == 0);
			});

		if (it != mMaterials.end())
		{
			return (*it).get();
		}

		return nullptr;
	}

	PBMaterial* ResourcesManager::GetOrCreatePBMaterial(const char* materialPath, const char* materialName)
	{
		auto it = std::find_if(
			mPBMaterials.begin(),
			mPBMaterials.end(),
			[materialPath, materialName](Handle<PBMaterial>& material) {
				return (material->mPath.compare(materialPath) == 0 && material->mName.compare(materialName) == 0);
			});

		if (it != mPBMaterials.end())
		{
			return (*it).get();
		}

		return nullptr;
	}

	Texture* ResourcesManager::GetOrCreateTexture(const char* texturePath)
	{
		auto it = std::find_if(
			mTextures.begin(),
			mTextures.end(),
			[texturePath](Handle<Texture>& texture) {
				return texture->mPath.compare(texturePath) == 0;
			});

		if (it != mTextures.end())
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
		return mTextures.emplace_back(std::move(texture)).get();
	}

	Model* ResourcesManager::GetOrCreateModel(const char* modelPath)
	{
		auto it = std::find_if(
			mModels.begin(),
			mModels.end(),
			[modelPath](Handle<Model>& model) {
				return model->mPath.compare(modelPath) == 0;
			});

		if (it != mModels.end())
		{
			return (*it).get();
		}

		if (mResourcesLoader->ParseModel(modelPath, *this))
		{
			return mModels.back().get();
		}

		return nullptr;
	}

	Skin* ResourcesManager::GetSkin(const char* path)
	{
		auto it = std::find_if(mSkins.begin(), mSkins.end(), [path](Handle<Skin>& skin) {
			return skin->mPath == path;
		});

		return (it != mSkins.end() ? (*it).get() : nullptr);
	}

	SkeletonAnimation* ResourcesManager::GetAnimation(const char* name)
	{
		auto it = std::find_if(mAnimations.begin(), mAnimations.end(), [name](Handle<SkeletonAnimation>& animation) {
			return animation->mName == name;
		});

		return (it != mAnimations.end() ? (*it).get() : nullptr);
	}

} // namespace erm
