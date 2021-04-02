#include "erm/loaders/obj/ObjModelLoader.h"

#include "erm/loaders/obj/ObjMaterialLoader.h"

#include "erm/managers/ResourcesManager.h"

#include "erm/math/math.h"

#include "erm/rendering/data_structs/IndexData.h"
#include "erm/rendering/data_structs/Model.h"
#include "erm/rendering/data_structs/VertexData.h"
#include "erm/rendering/materials/Material.h"

#include "erm/utils/Utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

namespace erm {

void ParseFace(
	std::vector<VertexData>& oVertices,
	const std::vector<PositionVertex>& pVertices,
	const std::vector<NormalVertex>& nVertices,
	const std::vector<UVVertex>& uvVertices,
	const std::vector<std::string>& splitted);

void Triangulate(
	const std::vector<VertexData>& vertices,
	std::vector<IndexData>& oIndices);

void AddMesh(
	std::mutex& mutex,
	Model& model,
	uint32_t& indicesOffset,
	std::vector<VertexData>&& vertices,
	std::vector<IndexData>&& indices,
	Material* material,
	std::string& meshName);

void ParseObjModel(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	Model& model,
	ResourcesManager& resourcesManager)
{
	std::ifstream stream(path);

	std::string line;
	std::string name;
	std::string meshName;
	std::vector<PositionVertex> pVertices;
	std::vector<NormalVertex> nVertices;
	std::vector<UVVertex> uvVertices;
	std::vector<VertexData> verticesData;
	std::vector<IndexData> indicesData;
	Material* material = nullptr;

	uint32_t indicesOffset = 0;
	bool wasLooping = false;
	bool noMat = false;

	while (std::getline(stream, line))
	{
		if (stop)
		{
			stream.close();
			return;
		}

		const std::vector<std::string> splitted = Utils::SplitString(line, ' ');

		if (splitted.size() > 0)
		{
			if (splitted[0].compare("o") == 0)
			{
				ASSERT(splitted.size() >= 2);
				name = splitted[splitted.size() - 1];

				if (!verticesData.empty() && !indicesData.empty())
				{
					AddMesh(mutex, model, indicesOffset, std::move(verticesData), std::move(indicesData), material, meshName);
					meshName.clear();
				}
			}
			else if (splitted[0].compare("g") == 0)
			{
				if (!verticesData.empty() && !indicesData.empty())
				{
					AddMesh(mutex, model, indicesOffset, std::move(verticesData), std::move(indicesData), material, meshName);
				}

				if (wasLooping)
				{
					wasLooping = false;
					verticesData.clear();
					indicesData.clear();
				}

				if (splitted.size() >= 2)
				{
					meshName = splitted[splitted.size() - 1];
				}
			}
			else if (splitted[0].compare("mtllib") == 0)
			{
				std::string pathStr("res/models/");
				pathStr.append(splitted[1]);
				noMat = !ParseMaterialsLib(mutex, stop, pathStr.c_str(), resourcesManager);
			}
			else if (splitted[0].compare("v") == 0)
			{
				if (!verticesData.empty() && !indicesData.empty() && material)
				{
					AddMesh(mutex, model, indicesOffset, std::move(verticesData), std::move(indicesData), material, meshName);
				}

				if (wasLooping)
				{
					wasLooping = false;
					verticesData.clear();
					indicesData.clear();
				}

				ASSERT(splitted.size() >= 4);
				pVertices.emplace_back(
					std::atof(splitted[splitted.size() - 3].c_str()),
					std::atof(splitted[splitted.size() - 2].c_str()),
					std::atof(splitted[splitted.size() - 1].c_str()));
			}
			else if (splitted[0].compare("vn") == 0)
			{
				ASSERT(splitted.size() >= 4);
				nVertices.emplace_back(
					std::atof(splitted[splitted.size() - 3].c_str()),
					std::atof(splitted[splitted.size() - 2].c_str()),
					std::atof(splitted[splitted.size() - 1].c_str()));
			}
			else if (splitted[0].compare("vt") == 0)
			{
				ASSERT(splitted.size() >= 3);
				uvVertices.emplace_back(
					std::atof(splitted[splitted.size() - 2].c_str()),
					std::atof(splitted[splitted.size() - 1].c_str()));
			}
			else if (splitted[0].compare("usemtl") == 0)
			{
				if (!verticesData.empty() && !indicesData.empty())
				{
					AddMesh(mutex, model, indicesOffset, std::move(verticesData), std::move(indicesData), material, meshName);
				}
				if (noMat)
				{
					material = &Material::DEFAULT;
					continue;
				}
				ASSERT(splitted.size() >= 2);
				std::string materialName = splitted[splitted.size() - 1];
				mutex.lock();
				const Materials& materials = resourcesManager.GetMaterials();
				auto it = std::find_if(materials.begin(), materials.end(), [materialName](const std::unique_ptr<Material>& material) {
					return material->mName.compare(materialName) == 0;
				});
				material = it != materials.end() ? (*it).get() : nullptr;
				mutex.unlock();
			}
			else if (splitted[0].compare("f") == 0)
			{
				wasLooping = true;

				std::vector<VertexData> vVertices;
				ParseFace(vVertices, pVertices, nVertices, uvVertices, splitted);

				std::vector<IndexData> vIndices;
				Triangulate(vVertices, vIndices);

				if (vIndices.empty())
				{
					continue;
				}

				for (unsigned int i = 0; i < vIndices.size(); ++i)
				{
					indicesData.emplace_back(indicesOffset + static_cast<IndexData>(verticesData.size() + vIndices[i]));
				}

				for (unsigned int i = 0; i < vVertices.size(); ++i)
				{
					verticesData.emplace_back(vVertices[i]);
				}
			}
		}
	}

	stream.close();

	if (meshName.find("Collider") == std::string::npos && !verticesData.empty() && !indicesData.empty())
	{
		AddMesh(mutex, model, indicesOffset, std::move(verticesData), std::move(indicesData), material, meshName);
	}

	if (!name.empty())
	{
		model.mName = name;
	}
}

void ParseFace(
	std::vector<VertexData>& oVertices,
	const std::vector<PositionVertex>& pVertices,
	const std::vector<NormalVertex>& nVertices,
	const std::vector<UVVertex>& uvVertices,
	const std::vector<std::string>& splitted)
{
	static constexpr unsigned int kVertexIndex = 0;
	static constexpr unsigned int kUVVertexIndex = 1;
	static constexpr unsigned int kNormalVertexIndex = 2;

	for (const std::string& split : splitted)
	{
		if (split.empty() || split.compare("f") == 0)
		{
			continue;
		}

		const std::vector<std::string> indices = Utils::SplitString(split, '/');
		ASSERT(indices.size() <= 3);
		VertexData vertex;

		const unsigned int positionIndex = std::atoi(indices[kVertexIndex].c_str()) - 1;
		int nPositionIndex = -1;
		int uvPositionIndex = -1;

		if (indices.size() == 2)
		{
			uvPositionIndex = std::atoi(indices[kUVVertexIndex].c_str()) - 1;
		}
		else if (indices.size() == 3)
		{
			nPositionIndex = std::atoi(indices[kNormalVertexIndex].c_str()) - 1;

			if (!indices[kUVVertexIndex].empty())
			{
				uvPositionIndex = std::atoi(indices[kUVVertexIndex].c_str()) - 1;
			}
		}

		ASSERT(positionIndex < pVertices.size());
		vertex.mPositionVertex = pVertices[positionIndex];

		if (nPositionIndex >= 0 && nPositionIndex < static_cast<int>(nVertices.size()))
		{
			ASSERT(nPositionIndex < static_cast<int>(nVertices.size()));
			vertex.mNormalVertex = nVertices[nPositionIndex];
		}

		if (uvPositionIndex >= 0 && uvPositionIndex < static_cast<int>(uvVertices.size()))
		{
			ASSERT(uvPositionIndex < static_cast<int>(uvVertices.size()));
			vertex.mUVVertex = uvVertices[uvPositionIndex];
		}

		oVertices.emplace_back(vertex);
	}
}

void Triangulate(
	const std::vector<VertexData>& vertices,
	std::vector<IndexData>& oIndices)
{
	if (vertices.size() < 3 || vertices.size() > 4)
	{
		return;
	}
	else if (vertices.size() == 3)
	{
		oIndices.push_back(0);
		oIndices.push_back(1);
		oIndices.push_back(2);
		return;
	}

	const PositionVertex& a = vertices[0].mPositionVertex;
	const PositionVertex& b = vertices[1].mPositionVertex;
	const PositionVertex& c = vertices[2].mPositionVertex;
	const PositionVertex& d = vertices[3].mPositionVertex;

	if ((a == b && d == c) ||
		(a == d && b == c) ||
		(a == c && d == b))
	{
		return;
	}

	oIndices.push_back(0);
	oIndices.push_back(1);
	oIndices.push_back(3);

	PositionVertex d1 = c - a;
	PositionVertex d2 = d - b;
	PositionVertex intersection;

	if (math::VerticesIntersection(a, b, d1, d2, intersection))
	{
		oIndices.push_back(1);
		oIndices.push_back(2);
		oIndices.push_back(3);
		return;
	}

	d1 = c - b;
	d2 = d - a;

	if (math::VerticesIntersection(b, a, d1, d2, intersection))
	{
		oIndices.push_back(0);
		oIndices.push_back(2);
		oIndices.push_back(3);
		return;
	}

	oIndices.push_back(0);
	oIndices.push_back(1);
	oIndices.push_back(2);
}

void AddMesh(
	std::mutex& mutex,
	Model& model,
	uint32_t& indicesOffset,
	std::vector<VertexData>&& vertices,
	std::vector<IndexData>&& indices,
	Material* material,
	std::string& meshName)
{
	indicesOffset += static_cast<uint32_t>(vertices.size());

	RenderConfigs configs = RenderConfigs::MODELS_RENDER_CONFIGS;
	configs.mMaterial.mType = MaterialType::LEGACY;
	configs.mMaterial.mData = material ? material : &Material::DEFAULT;

	mutex.lock();
	model.AddMesh(std::move(vertices), std::move(indices), configs, meshName.c_str());
	mutex.unlock();
}

} // namespace erm
