#include "erm/managers/ResourcesManager.h"

#include "erm/rendering/animations/SkeletonAnimation.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/data_structs/Skin.h"
#include "erm/rendering/materials/Material.h"
#include "erm/rendering/materials/PBMaterial.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/shaders/ShaderUtils.h"
#include "erm/rendering/textures/CubeMap.h"
#include "erm/rendering/textures/Texture.h"

#include "erm/utils/MeshUtils.h"
#include "erm/utils/Utils.h"

#include <algorithm>
#include <filesystem>

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
	const auto it = FindResourceByPath(mCubeMaps, path);

	if (it != mCubeMaps.end())
		return (*it).get();

	if (!std::filesystem::exists(path))
		return nullptr;

	Handle<CubeMap> result = std::make_unique<CubeMap>(mDevice);
	result->InitFromFile(path);
	return mCubeMaps.emplace_back(std::move(result)).get();
}

IShaderProgram* ResourcesManager::GetOrCreateShaderProgram(const char* shaderProgramPath)
{
	const auto it = FindResourceByPath(mShaderPrograms, shaderProgramPath);

	if (it != mShaderPrograms.end())
		return (*it).get();

	bool found = false;
	for (int i = 0; i < static_cast<int>(ShaderType::COUNT); ++i)
	{
		std::string path = ShaderUtils::GetShaderFilename(shaderProgramPath, 0, static_cast<ShaderType>(i));
		if (std::filesystem::exists(path))
		{
			found = true;
			break;
		}
	}

	if (!found)
		return nullptr;

	std::unique_ptr<ShaderProgram> shaderProgram = std::make_unique<ShaderProgram>(mDevice, shaderProgramPath);
	shaderProgram->Init();
	return mShaderPrograms.emplace_back(std::move(shaderProgram)).get();
}

Material* ResourcesManager::GetOrCreateMaterial(const char* materialPath, const char* materialName)
{
	const auto it = FindResourceByNameAndPath(mMaterials, materialPath, materialName);

	if (it != mMaterials.end())
	{
		return (*it).get();
	}

	return nullptr;
}

PBMaterial* ResourcesManager::GetOrCreatePBMaterial(const char* materialPath, const char* materialName)
{
	auto it = FindResourceByNameAndPath(mPBMaterials, materialPath, materialName);

	if (it != mPBMaterials.cend())
	{
		return (*it).get();
	}

	return nullptr;
}

Texture* ResourcesManager::GetOrCreateTexture(const char* texturePath)
{
	const auto it = FindResourceByPath(mTextures, texturePath);

	if (it != mTextures.end())
	{
		return (*it).get();
	}

	if (!std::filesystem::exists(texturePath))
		return nullptr;

	std::unique_ptr<Texture> texture = std::make_unique<Texture>(mDevice);
	texture->InitFromFile(texturePath);
	return mTextures.emplace_back(std::move(texture)).get();
}

Texture* ResourcesManager::CreateEmptyTexture()
{
	return mTextures.emplace_back(std::make_unique<Texture>(mDevice)).get();
}

void ResourcesManager::ReleaseTexture(const Texture* texture) 
{
	const auto it = std::find_if(mTextures.begin(), mTextures.end(), [texture](const Handle<Texture>& entry) {
		return entry.get() == texture;
	});

	if (it != mTextures.end())
	{
		mTextures.erase(it);
	}
}

Model* ResourcesManager::GetOrCreateModel(const char* modelPath)
{
	const auto it = FindResourceByPath(mModels, modelPath);

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
	const auto it = FindResourceByPath(mSkins, path);
	return (it != mSkins.end() ? (*it).get() : nullptr);
}

SkeletonAnimation* ResourcesManager::GetAnimation(const char* name)
{
	const auto it = FindResourceByName(mAnimations, name);
	return (it != mAnimations.end() ? (*it).get() : nullptr);
}

} // namespace erm
