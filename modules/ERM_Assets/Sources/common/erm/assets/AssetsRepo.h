#pragma once

#include "erm/assets/animations/SkeletonAnimation.h"
#include "erm/assets/data_structs/Skeleton.h"
#include "erm/assets/materials/Material.h"
#include "erm/assets/materials/PBMaterial.h"
#include "erm/assets/models/MeshUtils.h"
#include "erm/assets/models/Mesh.h"
#include "erm/assets/models/Model.h"
#include "erm/assets/shaders/ShaderProgram.h"
#include "erm/assets/shaders/ShaderUtils.h"
#include "erm/assets/textures/CubeMap.h"
#include "erm/assets/textures/Texture.h"

#include <erm/utils/assert/Assert.h>

#include <unordered_map>
#include <utility>

namespace erm {

class AssetsRepo
{
public:
	void loadDefaultResources();

	template<typename T>
	T* registerAsset(StringID assetID, T&& asset)
	{
		if (!assetID.isValid())
		{
			return nullptr;
		}
		
		auto& assets = getAllAssets<T>();
		ERM_ASSERT(assets.find(assetID) == assets.end());
		auto [value, success] = assets.try_emplace(assetID, std::move(asset));
		return success ? &(value->second) : nullptr;
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
		return it != assets.end() ? &(it->second) : nullptr;
	}
	
	template<typename T>
	const std::unordered_map<StringID, T>& getAllAssets() const
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
	std::unordered_map<StringID, T>& getAllAssets()
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
		else {
			static_assert(std::is_same_v<Mesh, T>);
		}
	}
	
	std::unordered_map<StringID, Mesh> mMeshes;
	std::unordered_map<StringID, Model> mModels;
	std::unordered_map<StringID, Texture> mTextures;
	std::unordered_map<StringID, CubeMap> mCubeMaps;
	std::unordered_map<StringID, Material> mMaterials;
	std::unordered_map<StringID, PBMaterial> mPBMaterials;
	std::unordered_map<StringID, Skeleton> mSkeletons;
	std::unordered_map<StringID, ShaderProgram> mShaderPrograms;
	std::unordered_map<StringID, SkeletonAnimation> mSkeletonAnimations;
	
};

} // namespace erm
