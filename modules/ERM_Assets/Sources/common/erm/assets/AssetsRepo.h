#pragma once

#include "erm/assets/animations/SkeletonAnimation.h"
#include "erm/assets/data_structs/Skeleton.h"
#include "erm/assets/materials/Material.h"
#include "erm/assets/materials/PBMaterial.h"
#include "erm/assets/models/Mesh.h"
#include "erm/assets/models/MeshUtils.h"
#include "erm/assets/models/Model.h"
#include "erm/assets/shaders/ShaderProgram.h"
#include "erm/assets/textures/CubeMap.h"
#include "erm/assets/textures/Texture.h"

#include <erm/log/Assert.h>
#include <erm/modules_lib/IModuleObject.h>
#include <erm/utils/StringID.h>

#include <refl.hpp>

#include <memory>
#include <unordered_map>

namespace erm {

class AssetsRepo : public IModuleObject<AssetsRepo>
{
public:
	void clearAll();

	template<typename T>
	T* registerAsset(StringID assetID, std::unique_ptr<T> asset)
	{
		if (!assetID.isValid())
		{
			return nullptr;
		}
		
		auto& assets = getAllAssets<T>();
		ERM_ASSERT(assets.find(assetID) == assets.end());
		auto [value, success] = assets.try_emplace(assetID, std::move(asset));
		return success ? value->second.get() : nullptr;
	}
	
	template<typename T>
	bool unregisterAsset(StringID assetID)
	{
		ERM_ASSERT(assetID.isValid());
		auto& assets = getAllAssets<T>();
		return assets.erase(assetID);
	}
	
	template<typename T>
	T* getAsset(StringID assetID)
	{
		if (!assetID.isValid())
		{
			return nullptr;
		}
		
		auto& assets = getAllAssets<T>();
		auto it = assets.find(assetID);
		return it != assets.end() ? it->second.get() : nullptr;
	}
	
	template<typename T>
	const std::unordered_map<StringID, std::unique_ptr<T>>& getAllAssetsOfType() const
	{
		if constexpr (std::is_same_v<Mesh, T>)
		{
			return mMeshes;
		}
		else if constexpr (std::is_same_v<Model, T>)
		{
			return mModels;
		}
		else if constexpr (std::is_same_v<Texture, T>)
		{
			return mTextures;
		}
		else if constexpr (std::is_same_v<CubeMap, T>)
		{
			return mCubeMaps;
		}
		else if constexpr (std::is_same_v<Material, T>)
		{
			return mMaterials;
		}
		else if constexpr (std::is_same_v<PBMaterial, T>)
		{
			return mPBMaterials;
		}
		else if constexpr (std::is_same_v<Skeleton, T>)
		{
			return mSkeletons;
		}
		else if constexpr (std::is_same_v<ShaderProgram, T>)
		{
			return mShaderPrograms;
		}
		else if constexpr (std::is_same_v<SkeletonAnimation, T>)
		{
			return mSkeletonAnimations;
		}
	}

private:
	template<typename T>
	std::unordered_map<StringID, std::unique_ptr<T>>& getAllAssets()
	{
		if constexpr (std::is_same_v<Mesh, T>)
		{
			return mMeshes;
		}
		else if constexpr (std::is_same_v<Model, T>)
		{
			return mModels;
		}
		else if constexpr (std::is_same_v<Texture, T>)
		{
			return mTextures;
		}
		else if constexpr (std::is_same_v<CubeMap, T>)
		{
			return mCubeMaps;
		}
		else if constexpr (std::is_same_v<Material, T>)
		{
			return mMaterials;
		}
		else if constexpr (std::is_same_v<PBMaterial, T>)
		{
			return mPBMaterials;
		}
		else if constexpr (std::is_same_v<Skeleton, T>)
		{
			return mSkeletons;
		}
		else if constexpr (std::is_same_v<ShaderProgram, T>)
		{
			return mShaderPrograms;
		}
		else if constexpr (std::is_same_v<SkeletonAnimation, T>)
		{
			return mSkeletonAnimations;
		}
	}

	template<typename F>
	void forEachAssets(F&& function)
	{
		function(mMeshes);
		function(mModels);
		function(mTextures);
		function(mCubeMaps);
		function(mMaterials);
		function(mPBMaterials);
		function(mSkeletons);
		function(mShaderPrograms);
		function(mSkeletonAnimations);
	}

	std::unordered_map<StringID, std::unique_ptr<Mesh>> mMeshes;
	std::unordered_map<StringID, std::unique_ptr<Model>> mModels;
	std::unordered_map<StringID, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<StringID, std::unique_ptr<CubeMap>> mCubeMaps;
	std::unordered_map<StringID, std::unique_ptr<Material>> mMaterials;
	std::unordered_map<StringID, std::unique_ptr<PBMaterial>> mPBMaterials;
	std::unordered_map<StringID, std::unique_ptr<Skeleton>> mSkeletons;
	std::unordered_map<StringID, std::unique_ptr<ShaderProgram>> mShaderPrograms;
	std::unordered_map<StringID, std::unique_ptr<SkeletonAnimation>> mSkeletonAnimations;
	
};

} // namespace erm

REFL_AUTO(type(erm::AssetsRepo));
