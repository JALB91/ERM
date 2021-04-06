#ifdef ERM_ASSIMP_ENABLED

#	include "erm/loaders/assimp/AssimpModelLoader.h"

#	include "erm/managers/ResourcesManager.h"

#	include "erm/rendering/buffers/IndexData.h"
#	include "erm/rendering/buffers/VertexData.h"
#	include "erm/rendering/data_structs/Model.h"

#	include <assimp/Importer.hpp>
#	include <assimp/postprocess.h>
#	include <assimp/scene.h>

#	include <vector>

namespace erm {

math::vec3 ToVec3(const aiColor3D& color)
{
	return math::vec3(color.r, color.g, color.b);
}

math::vec3 ToVec3(const aiVector3D& vec)
{
	return math::vec3(vec.x, vec.y, vec.z);
}

math::vec2 ToVec2(const aiVector3D& vec)
{
	return math::vec2(vec.x, vec.y);
}

math::vec2 ToVec2(const aiVector2D& vec)
{
	return math::vec2(vec.x, vec.y);
}

void ProcessNode(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	Model& model,
	ResourcesManager& resourcesManager,
	const aiScene* scene,
	const aiNode* node,
	const std::vector<Material*>& materials,
	const std::vector<PBMaterial*>& pbMaterials);

void ProcessMeshes(
	std::mutex& mutex,
	Model& model,
	const aiScene* scene,
	const aiNode* node,
	const std::vector<Material*>& materials,
	const std::vector<PBMaterial*>& pbMaterials);

void AssimpParseModel(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	Model& model,
	ResourcesManager& resourcesManager)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_SortByPType);

	if (!scene || !scene->mRootNode)
		return;

	std::vector<Material*> materials;
	std::vector<PBMaterial*> pbMaterials;

	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* mat = scene->mMaterials[i];
		Material material = Material::DEFAULT;

		aiColor3D diffuse;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
			material.mDiffuse = ToVec3(diffuse);

		aiColor3D specular;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, specular))
			material.mSpecular = ToVec3(specular);

		aiColor3D ambient;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
			material.mAmbient = ToVec3(ambient);

		float shininess;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, shininess))
			material.mShininess = shininess;

		aiString name;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_NAME, name))
			material.mName = name.C_Str();

		aiString diffuseMap;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(0, aiTextureType_DIFFUSE), diffuseMap))
			material.mDiffuseMap = resourcesManager.GetOrCreateTexture((std::string("res/textures/") + diffuseMap.C_Str()).c_str());

		aiString normalMap;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(0, aiTextureType_NORMALS), normalMap))
			material.mNormalMap = resourcesManager.GetOrCreateTexture((std::string("res/textures/") + normalMap.C_Str()).c_str());

		aiString specularMap;
		if (AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(0, aiTextureType_SPECULAR), specularMap))
			material.mSpecularMap = resourcesManager.GetOrCreateTexture((std::string("res/textures/") + specularMap.C_Str()).c_str());

		material.mPath = path;

		PBMaterial pbMat = PBMaterial::DEFAULT;
		pbMat.mPath = material.mPath;
		pbMat.mName = material.mName;
		pbMat.mAlbedo = material.mDiffuse;

		mutex.lock();
		pbMaterials.emplace_back(resourcesManager.GetPBMaterials().emplace_back(std::make_unique<PBMaterial>(std::move(pbMat))).get());
		materials.emplace_back(resourcesManager.GetMaterials().emplace_back(std::make_unique<Material>(std::move(material))).get());
		mutex.unlock();
	}

	ProcessNode(mutex, stop, model, resourcesManager, scene, scene->mRootNode, materials, pbMaterials);
}

void ProcessNode(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	Model& model,
	ResourcesManager& resourcesManager,
	const aiScene* scene,
	const aiNode* node,
	const std::vector<Material*>& materials,
	const std::vector<PBMaterial*>& pbMaterials)
{
	ProcessMeshes(mutex, model, scene, node, materials, pbMaterials);
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		if (stop)
			break;
		ProcessNode(mutex, stop, model, resourcesManager, scene, node->mChildren[i], materials, pbMaterials);
	}
}

void ProcessMeshes(
	std::mutex& mutex,
	Model& model,
	const aiScene* scene,
	const aiNode* node,
	const std::vector<Material*>& materials,
	const std::vector<PBMaterial*>& pbMaterials)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		std::vector<VertexData> vData(mesh->mNumVertices);

		for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
		{
			vData[j].mPositionVertex = ToVec3(mesh->mVertices[j]);
			if (mesh->HasNormals() && mesh->mNormals)
				vData[j].mNormalVertex = ToVec3(mesh->mNormals[j]);
			if (mesh->HasTextureCoords(0) && mesh->mTextureCoords)
				vData[j].mUVVertex = ToVec2(mesh->mTextureCoords[0][j]);
		}

		std::vector<IndexData> iData;

		for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
		{
			const aiFace& face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; ++k)
			{
				iData.emplace_back(face.mIndices[k]);
			}
		}

		PipelineConfigs config = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS;

		if (mesh->mMaterialIndex < pbMaterials.size())
			config.mPBMaterial = pbMaterials[mesh->mMaterialIndex];
		if (mesh->mMaterialIndex < materials.size())
			config.mMaterial = materials[mesh->mMaterialIndex];

		mutex.lock();
		model.AddMesh(std::move(vData), std::move(iData), config, mesh->mName.C_Str());
		mutex.unlock();
	}
}

} // namespace erm

#endif
