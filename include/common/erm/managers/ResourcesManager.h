#pragma once

#include "erm/rendering/animations/SkeletonAnimation.h"
#include "erm/rendering/data_structs/Material.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/data_structs/PBMaterial.h"
#include "erm/rendering/data_structs/Skin.h"
#include "erm/rendering/shaders/ShaderProgram.h"
#include "erm/rendering/textures/Texture.h"

#include <memory>
#include <vector>

namespace erm {
	class Device;
	class ResourcesLoader;
} // namespace erm

namespace erm {

	template<typename T>
	using Handle = std::unique_ptr<T>;

	using Shaders = std::vector<Handle<ShaderProgram>>;
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
		void OnRender();
		void OnPostRender();

		inline Shaders& GetShaderPrograms() { return mShaderPrograms; }
		ShaderProgram* GetOrCreateShaderProgram(const char* vertexShader, const char* fragmentShader);
		ShaderProgram* GetOrCreateShaderProgram(const char* shaderProgramPath);

		inline Materials& GetMaterials() { return mMaterials; }
		Material* GetOrCreateMaterial(const char* materialPath, const char* materialName);

		inline PBMaterials& GetPBMaterials() { return mPBMaterials; }
		PBMaterial* GetOrCreatePBMaterial(const char* materialPath, const char* materialName);

		inline Textures& GetTextures() { return mTextures; }
		Texture* GetOrCreateTexture(const char* texturePath);

		inline Models& GetModels() { return mModels; }
		Model* GetOrCreateModel(const char* modelPath);

		inline Skins& GetSkins() { return mSkins; }
		Skin* GetSkin(const char* name);

		inline Animations& GetAnimations() { return mAnimations; }
		SkeletonAnimation* GetAnimation(const char* name);

	private:
		Device& mDevice;
		std::unique_ptr<ResourcesLoader> mResourcesLoader;

		Shaders mShaderPrograms;
		Materials mMaterials;
		PBMaterials mPBMaterials;
		Textures mTextures;
		Models mModels;
		Skins mSkins;
		Animations mAnimations;
	};

} // namespace erm
