#pragma once

#include "erm/rendering/animations/SkeletonAnimation.h"
#include "erm/rendering/data_structs/Bone.h"
#include "erm/rendering/data_structs/Material.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

#include <vector>
#include <memory>

namespace erm {
	
	class ResourcesLoader;
	
	template<typename T>
	using Handle = std::unique_ptr<T>;
	
	using Shaders = std::vector<Handle<ShaderProgram>>;
	using Materials = std::vector<Handle<Material>>;
	using Textures = std::vector<Handle<Texture>>;
	using Models = std::vector<Handle<Model>>;
	using Skins = std::vector<Handle<BonesTree>>;
	using Animations = std::vector<Handle<SkeletonAnimation>>;
	
	class ResourcesManager
	{
	public:
		ResourcesManager();
		~ResourcesManager();
		
		void LoadDefaultResources();
		
		void OnUpdate();
		void OnRender();
		void OnPostRender();
		
		inline Shaders& GetLoadedShaderPrograms() { return mLoadedShaderPrograms; }
		ShaderProgram* GetOrCreateShaderProgram(const char* vertexShader, const char* fragmentShader);
		ShaderProgram* GetOrCreateShaderProgram(const char* shaderProgramPath);
		
		inline Materials& GetLoadedMaterials() { return mLoadedMaterials; }
		Material* GetOrCreateMaterial(const char* materialPath, const char* materialName);
		
		inline Textures& GetLoadedTextures() { return mLoadedTextures; }
		Texture* GetOrCreateTexture(const char* texturePath);
		
		inline Models& GetLoadedModels() { return mLoadedModels; }
		Model* GetOrCreateModel(const char* modelPath);
		
		inline Skins& GetLoadedSkins() { return mLoadedSkins; }
		BonesTree* GetSkin(const char* name);
		
		inline Animations& GetLoadedAnimations() { return mLoadedAnimations; }
		SkeletonAnimation* GetAnimation(const char* name);
		
	private:
		std::unique_ptr<ResourcesLoader> mResourcesLoader;
		
		Shaders mLoadedShaderPrograms;
		Materials mLoadedMaterials;
		Textures mLoadedTextures;
		Models mLoadedModels;
		Skins mLoadedSkins;
		Animations mLoadedAnimations;
		
	};
	
}
