#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/animations/SkeletonAnimation.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/data_structs/Skin.h"
#include "erm/rendering/enums/ShaderType.h"
#include "erm/rendering/materials/Material.h"
#include "erm/rendering/materials/PBMaterial.h"
#include "erm/rendering/shaders/ShaderUtils.h"
#if defined(ERM_VULKAN)
#	include "erm/rendering/shaders/VulkanShaderProgram.h"
#endif
#include "erm/rendering/textures/CubeMap.h"
#include "erm/rendering/textures/Texture.h"

#include "erm/utils/MeshUtils.h"
#include "erm/utils/Utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace erm {

ResourcesManager::ResourcesManager(Device& device)
	: mDevice(device)
	, mResourcesLoader(mDevice)
{}

ResourcesManager::~ResourcesManager()
{}

void ResourcesManager::LoadDefaultResources()
{
	mModels.reserve(10);
	mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Triangle", "Triangle"));
	mModels.back()->AddMesh(MeshUtils::CreateTriangle());
	mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Square", "Square"));
	mModels.back()->AddMesh(MeshUtils::CreateSquare());
	mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Cube", "Cube"));
	mModels.back()->AddMesh(MeshUtils::CreateCube());
	mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Sphere", "Sphere"));
	mModels.back()->AddMesh(MeshUtils::CreateSphere());
	mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Spike", "Spike"));
	mModels.back()->AddMesh(MeshUtils::CreateSpike());
	mModels.emplace_back(std::make_unique<Model>(mDevice, "Defaults/Grid", "Grid"));
	mModels.back()->AddMesh(MeshUtils::CreateGrid());

	for (const auto& model : mModels)
		model->UpdateBuffers();
}

void ResourcesManager::OnUpdate()
{
	mResourcesLoader.OnUpdate();
}

void ResourcesManager::OnPreRender()
{
	mResourcesLoader.OnPreRender();
}

void ResourcesManager::OnPostRender()
{
	mResourcesLoader.OnPostRender();
}

bool ResourcesManager::IsStillLoading(const Model& model) const
{
	return mResourcesLoader.IsStillLoading(model);
}

CubeMap* ResourcesManager::GetOrCreateCubeMap(const char* path)
{
	auto it = std::find_if(
		mCubeMaps.begin(),
		mCubeMaps.end(),
		[path](Handle<CubeMap>& cubeMap) {
			return cubeMap->mPath == path;
		});

	if (it != mCubeMaps.end())
		return (*it).get();

	Handle<CubeMap> result = std::make_unique<CubeMap>(mDevice, path);
	result->Init();
	return mCubeMaps.emplace_back(std::move(result)).get();
}

IShaderProgram* ResourcesManager::GetOrCreateShaderProgram(const char* shaderProgramPath)
{
	auto it = std::find_if(
		mShaderPrograms.begin(),
		mShaderPrograms.end(),
		[shaderProgramPath](Handle<IShaderProgram>& program) {
			return program->mPath.compare(shaderProgramPath) == 0;
		});

	if (it != mShaderPrograms.end())
		return (*it).get();

#if defined(ERM_VULKAN)
	std::unique_ptr<VulkanShaderProgram> shaderProgram = std::make_unique<VulkanShaderProgram>(mDevice, shaderProgramPath);
#endif
	shaderProgram->Init();
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
	texture->Init();
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

	if (mResourcesLoader.ParseModel(modelPath, *this))
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
