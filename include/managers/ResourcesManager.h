#pragma once

#include <deque>

namespace erm {
	
	class ShaderProgram;
	struct Material;
	class Texture;
	class Model;
	
	class ResourcesManager
	{
	public:
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
