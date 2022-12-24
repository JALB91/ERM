#pragma once

#include "erm/resources/loaders/ResourcesLoader.h"

#include <memory>
#include <string_view>
#include <vector>

namespace erm {
class ShaderProgram;
class Texture;
class Model;
class CubeMap;
class Mesh;
struct PBMaterial;
struct Material;
struct SkeletonAnimation;
struct Skin;
} // namespace erm

namespace erm {

template<typename T>
using Handle = std::unique_ptr<T>;

using CubeMaps = std::vector<Handle<CubeMap>>;
using Shaders = std::vector<Handle<ShaderProgram>>;
using Materials = std::vector<Handle<Material>>;
using PBMaterials = std::vector<Handle<PBMaterial>>;
using Textures = std::vector<Handle<Texture>>;
using Models = std::vector<Handle<Model>>;
using Skins = std::vector<Handle<Skin>>;
using Animations = std::vector<Handle<SkeletonAnimation>>;
using Meshes = std::vector<Handle<Mesh>>;

class ResourcesManager
{
public:
	ResourcesManager();
	~ResourcesManager();

	void LoadDefaultResources();

	void OnUpdate();
	void OnPreRender();
	void OnPostRender();

	bool IsStillLoading(const Model& model) const;

	inline Shaders& GetShaderPrograms() { return mShaderPrograms; }
	ShaderProgram* GetOrCreateShaderProgram(std::string_view shaderProgramPath);

	inline Materials& GetMaterials() { return mMaterials; }
	Material* GetOrCreateMaterial(std::string_view materialPath, std::string_view materialName);

	inline PBMaterials& GetPBMaterials() { return mPBMaterials; }
	PBMaterial* GetOrCreatePBMaterial(std::string_view materialPath, std::string_view materialName);

	inline Textures& GetTextures() { return mTextures; }
	Texture* GetOrCreateTexture(std::string_view texturePath);
	Texture* CreateEmptyTexture(std::string_view name, uint32_t width, uint32_t height);
	void ReleaseTexture(const Texture* texture);

	inline CubeMaps& GetCubeMaps() { return mCubeMaps; }
	CubeMap* GetOrCreateCubeMap(std::string_view path);

	inline Models& GetModels() { return mModels; }
	Model* GetOrCreateModel(std::string_view modelPath);

	inline Skins& GetSkins() { return mSkins; }
	Skin* GetSkin(std::string_view name);

	inline Animations& GetAnimations() { return mAnimations; }
	SkeletonAnimation* GetAnimation(std::string_view name);
	
	inline Meshes& GetMeshes() { return mMeshes; }
	Mesh* GetMesh(std::string_view name);

private:
	template<typename T>
	typename T::const_iterator FindResourceByPath(const T& vector, std::string_view path)
	{
		return std::find_if(
			vector.begin(),
			vector.end(),
			[path](const auto& resource) {
				return resource->mPath.compare(path) == 0;
			});
	}

	template<typename T>
	typename T::const_iterator FindResourceByName(const T& vector, std::string_view name)
	{
		return std::find_if(
			vector.begin(),
			vector.end(),
			[name](const auto& resource) {
				return resource->mName.compare(name) == 0;
			});
	}

	template<typename T>
	typename T::const_iterator FindResourceByNameAndPath(const T& vector, std::string_view path, std::string_view name)
	{
		return std::find_if(
			vector.begin(),
			vector.end(),
			[path, name](const auto& resource) {
				return resource->mPath.compare(path) == 0 && resource->mName.compare(name) == 0;
			});
	}
	
	CubeMaps mCubeMaps;
	Shaders mShaderPrograms;
	Materials mMaterials;
	PBMaterials mPBMaterials;
	Textures mTextures;
	Models mModels;
	Skins mSkins;
	Animations mAnimations;
	Meshes mMeshes;
	
	ResourcesLoader mResourcesLoader;
};

} // namespace erm
