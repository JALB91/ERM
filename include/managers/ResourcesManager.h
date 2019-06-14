#pragma once

#include "rendering/ShaderProgram.h"
#include "rendering/Material.h"
#include "rendering/Texture.h"
#include "rendering/Model.h"

#include <deque>

namespace erm {
	
	class ResourcesManager
	{
	public:
		static void OnPostUpdate();
		
		static inline std::deque<ShaderProgram>& GetLoadedShaderPrograms() { return mLoadedShaderPrograms; }
		static ShaderProgram* GetOrCreateShaderProgram(const char* shaderProgramPath);
		
		static inline std::deque<Material>& GetLoadedMaterials() { return mLoadedMaterials; }
		static Material* GetOrCreateMaterial(const char* materialPath, const char* materialName);
		
		static inline std::deque<Texture>& GetLoadedTextures() { return mLoadedTextures; }
		static Texture* GetOrCreateTexture(const char* texturePath);
		
		static inline std::deque<Model>& GetLoadedModels() { return mLoadedModels; }
		static Model* GetOrCreateModel(const char* modelPath);
		
	private:
		static std::deque<ShaderProgram> mLoadedShaderPrograms;
		static std::deque<Material> mLoadedMaterials;
		static std::deque<Texture> mLoadedTextures;
		static std::deque<Model> mLoadedModels;
		
	};
	
}
