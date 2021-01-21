#ifdef ERM_FBX_ENABLED

#	include "erm/loaders/fbx/FBXMaterialLoader.h"

#	include "erm/managers/ResourcesManager.h"

namespace erm {

	const char* GetTextureName(FbxProperty& pProperty);

	Material* ParseFBXMaterial(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		FbxMesh* pMesh,
		ResourcesManager& resourcesManager)
	{
		FbxGeometryElementMaterial* lMaterialElement = pMesh->GetElementMaterial(0);
		FbxSurfaceMaterial* lMaterial = pMesh->GetNode()->GetMaterial(lMaterialElement->GetIndexArray().GetAt(0));
		int lMatId = lMaterialElement->GetIndexArray().GetAt(0);

		if (lMatId >= 0 && lMaterial)
		{
			mutex.lock();
			Materials& materials = resourcesManager.GetMaterials();
			auto it = std::find_if(materials.begin(), materials.end(), [&path, name = lMaterial->GetName()](const std::unique_ptr<Material>& material) {
				return material->mPath == path && material->mName == name;
			});
			if (it != materials.end())
			{
				Material* mat = it->get();
				mutex.unlock();
				return mat;
			}
			mutex.unlock();

			Material mat = Material::DEFAULT;
			mat.mPath = path;
			mat.mName = lMaterial->GetName();

			if (const char* name = GetTextureName(lMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse)))
				mat.mDiffuseMap = resourcesManager.GetOrCreateTexture((std::string("res/textures/") + name).c_str());

			if (const char* name = GetTextureName(lMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap)))
				mat.mNormalMap = resourcesManager.GetOrCreateTexture((std::string("res/textures/") + name).c_str());

			mutex.lock();
			Material* ret = materials.emplace_back(std::make_unique<Material>(std::move(mat))).get();
			mutex.unlock();
			return ret;
		}

		return nullptr;
	}

	const char* GetTextureName(FbxProperty& pProperty)
	{
		int lLayeredTextureCount = pProperty.GetSrcObjectCount<FbxLayeredTexture>();
		if (lLayeredTextureCount > 0)
		{
			for (int j = 0; j < lLayeredTextureCount; ++j)
			{
				FbxLayeredTexture* lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
				int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();

				for (int k = 0; k < lNbTextures; ++k)
				{
					return lLayeredTexture->GetName();
				}
			}
		}
		else
		{
			//no layered texture simply get on the property
			int lNbTextures = pProperty.GetSrcObjectCount<FbxTexture>();

			if (lNbTextures > 0)
			{
				for (int j = 0; j < lNbTextures; ++j)
				{
					FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
					if (lTexture)
					{
						return lTexture->GetName();
					}
				}
			}
		}

		return nullptr;
	}

} // namespace erm

#endif