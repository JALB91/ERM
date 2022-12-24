#include "erm/resources/ResourcesManager.h"

#include "erm/resources/animations/SkeletonAnimation.h"
#include "erm/resources/models/Model.h"
#include "erm/resources/data_structs/Skin.h"
#include "erm/resources/materials/Material.h"
#include "erm/resources/materials/PBMaterial.h"
#include "erm/resources/shaders/ShaderProgram.h"
#include "erm/resources/shaders/ShaderUtils.h"
#include "erm/resources/textures/CubeMap.h"
#include "erm/resources/textures/Texture.h"
#include "erm/resources/models/MeshUtils.h"
#include "erm/resources/models/Mesh.h"

#include <erm/utils/Utils.h>

#include <algorithm>
#include <filesystem>

namespace erm {

ResourcesManager::ResourcesManager()
{}

ResourcesManager::~ResourcesManager()
{}

void ResourcesManager::LoadDefaultResources()
{
	mModels.reserve(10);
	mMeshes.reserve(10);
	{
		auto& mesh = mMeshes.emplace_back(std::make_unique<Mesh>(MeshUtils::CreateTriangle()));
		auto& model = mModels.emplace_back(std::make_unique<Model>("Defaults/Triangle", "Triangle"));
		model->AddMesh(*mesh);
	}
	{
		auto& mesh = mMeshes.emplace_back(std::make_unique<Mesh>(MeshUtils::CreateSquare()));
		auto& model = mModels.emplace_back(std::make_unique<Model>("Defaults/Square", "Square"));
		model->AddMesh(*mesh);
	}
	{
		auto& mesh = mMeshes.emplace_back(std::make_unique<Mesh>(MeshUtils::CreateCube()));
		auto& model = mModels.emplace_back(std::make_unique<Model>("Defaults/Cube", "Cube"));
		model->AddMesh(*mesh);
	}
	{
		auto& mesh = mMeshes.emplace_back(std::make_unique<Mesh>(MeshUtils::CreateSphere()));
		auto& model = mModels.emplace_back(std::make_unique<Model>("Defaults/Sphere", "Sphere"));
		model->AddMesh(*mesh);
	}
	{
		auto& mesh = mMeshes.emplace_back(std::make_unique<Mesh>(MeshUtils::CreateSpike()));
		auto& model = mModels.emplace_back(std::make_unique<Model>("Defaults/Spike", "Spike"));
		model->AddMesh(*mesh);
	}
	{
		auto& mesh = mMeshes.emplace_back(std::make_unique<Mesh>(MeshUtils::CreateGrid()));
		auto& model = mModels.emplace_back(std::make_unique<Model>("Defaults/Grid", "Grid"));
		model->AddMesh(*mesh);
	}
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

CubeMap* ResourcesManager::GetOrCreateCubeMap(std::string_view path)
{
	const auto it = FindResourceByPath(mCubeMaps, path);

	if (it != mCubeMaps.end())
		return (*it).get();

	if (!std::filesystem::exists(path))
		return nullptr;

	return mCubeMaps.emplace_back(std::make_unique<CubeMap>(path)).get();
}

ShaderProgram* ResourcesManager::GetOrCreateShaderProgram(std::string_view shaderProgramPath)
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

	std::unique_ptr<ShaderProgram> shaderProgram = std::make_unique<ShaderProgram>(shaderProgramPath);
	return mShaderPrograms.emplace_back(std::move(shaderProgram)).get();
}

Material* ResourcesManager::GetOrCreateMaterial(std::string_view materialPath, std::string_view materialName)
{
	const auto it = FindResourceByNameAndPath(mMaterials, materialPath, materialName);

	if (it != mMaterials.end())
	{
		return (*it).get();
	}

	return nullptr;
}

PBMaterial* ResourcesManager::GetOrCreatePBMaterial(std::string_view materialPath, std::string_view materialName)
{
	auto it = FindResourceByNameAndPath(mPBMaterials, materialPath, materialName);

	if (it != mPBMaterials.cend())
	{
		return (*it).get();
	}

	return nullptr;
}

Texture* ResourcesManager::GetOrCreateTexture(std::string_view texturePath)
{
	const auto it = FindResourceByPath(mTextures, texturePath);

	if (it != mTextures.end())
	{
		return (*it).get();
	}

	if (!std::filesystem::exists(texturePath))
		return nullptr;

	return mTextures.emplace_back(std::make_unique<Texture>(texturePath)).get();
}

Texture* ResourcesManager::CreateEmptyTexture(std::string_view name, uint32_t width, uint32_t height)
{
	const auto it = FindResourceByName(mTextures, name);
	ERM_ASSERT(it == mTextures.end());
	return mTextures.emplace_back(std::make_unique<Texture>(name, width, height)).get();
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

Model* ResourcesManager::GetOrCreateModel(std::string_view modelPath)
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

Skin* ResourcesManager::GetSkin(std::string_view path)
{
	const auto it = FindResourceByPath(mSkins, path);
	return (it != mSkins.end() ? (*it).get() : nullptr);
}

SkeletonAnimation* ResourcesManager::GetAnimation(std::string_view name)
{
	const auto it = FindResourceByName(mAnimations, name);
	return (it != mAnimations.end() ? (*it).get() : nullptr);
}

} // namespace erm
