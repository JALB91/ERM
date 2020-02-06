#pragma once

#include "rendering/ShaderProgram.h"
#include "rendering/Material.h"
#include "rendering/Texture.h"
#include "rendering/Model.h"

#include <vector>
#include <memory>

namespace erm {
	
	template<typename T>
	using Handle = std::unique_ptr<T>;
	
	using Shaders = std::vector<Handle<ShaderProgram>>;
	using Materials = std::vector<Handle<Material>>;
	using Textures = std::vector<Handle<Texture>>;
	using Models = std::vector<Handle<Model>>;
	
	class ResourcesManager
	{
	public:
		ResourcesManager();
		~ResourcesManager();
		
		inline Shaders& GetLoadedShaderPrograms() { return mLoadedShaderPrograms; }
		ShaderProgram* GetOrCreateShaderProgram(const char* vertexShader, const char* fragmentShader);
		ShaderProgram* GetOrCreateShaderProgram(const char* shaderProgramPath);
		
		inline Materials& GetLoadedMaterials() { return mLoadedMaterials; }
		Material* GetOrCreateMaterial(const char* materialPath, const char* materialName);
		
		inline Textures& GetLoadedTextures() { return mLoadedTextures; }
		Texture* GetOrCreateTexture(const char* texturePath);
		
		inline Models& GetLoadedModels() { return mLoadedModels; }
		Model* GetOrCreateModel(const char* modelPath);
		
	private:
		Shaders mLoadedShaderPrograms;
		Materials mLoadedMaterials;
		Textures mLoadedTextures;
		Models mLoadedModels;
		
	};
	
}
