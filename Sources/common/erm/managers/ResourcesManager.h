#pragma once

#include "erm/loaders/ResourcesLoader.h"

#include <memory>
#include <vector>

namespace erm {
class Device;
class CubeMap;
class IShaderProgram;
class Texture;
class Model;
struct PBMaterial;
struct Material;
struct SkeletonAnimation;
struct Skin;
} // namespace erm

namespace erm {

template<typename T>
using Handle = std::unique_ptr<T>;

using CubeMaps = std::vector<Handle<CubeMap>>;
using Shaders = std::vector<Handle<IShaderProgram>>;
using Materials = std::vector<Handle<Material>>;
using PBMaterials = std::vector<Handle<PBMaterial>>;
using Textures = std::vector<Handle<Texture>>;
using Models = std::vector<Handle<Model>>;
using Skins = std::vector<Handle<Skin>>;
using Animations = std::vector<Handle<SkeletonAnimation>>;

class ResourcesManager
{
public:
	ResourcesManager(Device& device);
	~ResourcesManager();

	void LoadDefaultResources();

	void OnUpdate();
	void OnPreRender();
	void OnPostRender();

	bool IsStillLoading(const Model& model) const;

	inline Shaders& GetShaderPrograms() { return mShaderPrograms; }
	IShaderProgram* GetOrCreateShaderProgram(const char* shaderProgramPath);

	inline Materials& GetMaterials() { return mMaterials; }
	Material* GetOrCreateMaterial(const char* materialPath, const char* materialName);

	inline PBMaterials& GetPBMaterials() { return mPBMaterials; }
	PBMaterial* GetOrCreatePBMaterial(const char* materialPath, const char* materialName);

	inline Textures& GetTextures() { return mTextures; }
	Texture* GetOrCreateTexture(const char* texturePath);
	Texture* CreateEmptyTexture();
	void ReleaseTexture(const Texture* texture);

	inline CubeMaps& GetCubeMaps() { return mCubeMaps; }
	CubeMap* GetOrCreateCubeMap(const char* path);

	inline Models& GetModels() { return mModels; }
	Model* GetOrCreateModel(const char* modelPath);

	inline Skins& GetSkins() { return mSkins; }
	Skin* GetSkin(const char* name);

	inline Animations& GetAnimations() { return mAnimations; }
	SkeletonAnimation* GetAnimation(const char* name);

private:
	template<typename T>
	typename T::const_iterator FindResourceByPath(const T& vector, const char* path)
	{
		return std::find_if(
			vector.begin(),
			vector.end(),
			[path](const auto& resource) {
				return resource->mPath.compare(path) == 0;
			});
	}

	template<typename T>
	typename T::const_iterator FindResourceByName(const T& vector, const char* name)
	{
		return std::find_if(
			vector.begin(),
			vector.end(),
			[name](const auto& resource) {
				return resource->mName.compare(name) == 0;
			});
	}

	template<typename T>
	typename T::const_iterator FindResourceByNameAndPath(const T& vector, const char* path, const char* name)
	{
		return std::find_if(
			vector.begin(),
			vector.end(),
			[path, name](const auto& resource) {
				return resource->mPath.compare(path) == 0 && resource->mName.compare(name) == 0;
			});
	}
	
	Device& mDevice;
	ResourcesLoader mResourcesLoader;

	CubeMaps mCubeMaps;
	Shaders mShaderPrograms;
	Materials mMaterials;
	PBMaterials mPBMaterials;
	Textures mTextures;
	Models mModels;
	Skins mSkins;
	Animations mAnimations;
};

} // namespace erm
