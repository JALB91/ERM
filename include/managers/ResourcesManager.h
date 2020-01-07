#pragma once

#include "rendering/ShaderProgram.h"
#include "rendering/Material.h"
#include "rendering/Texture.h"
#include "rendering/Model.h"

#include <vector>
#include <memory>

namespace erm {
	
	class ResourcesManager
	{
	public:
		template<typename T>
		using Handle = std::unique_ptr<T>;
		
		using Shaders = std::vector<Handle<ShaderProgram>>;
		using Materials = std::vector<Handle<Material>>;
		using Textures = std::vector<Handle<Texture>>;
		using Models = std::vector<Handle<Model>>;
		
	public:
		static inline Shaders& GetLoadedShaderPrograms() { return mLoadedShaderPrograms; }
		static ShaderProgram* GetOrCreateShaderProgram(const char* shaderProgramPath);
		
		static inline Materials& GetLoadedMaterials() { return mLoadedMaterials; }
		static Material* GetOrCreateMaterial(const char* materialPath, const char* materialName);
		
		static inline Textures& GetLoadedTextures() { return mLoadedTextures; }
		static Texture* GetOrCreateTexture(const char* texturePath);
		
		static inline Models& GetLoadedModels() { return mLoadedModels; }
		static Model* GetOrCreateModel(const char* modelPath);
		
	private:
		static Shaders mLoadedShaderPrograms;
		static Materials mLoadedMaterials;
		static Textures mLoadedTextures;
		static Models mLoadedModels;
		
	};
	
}
