#include "utils/ModelUtils.h"

#include "rendering/Mesh.h"
#include "rendering/Model.h"
#include "rendering/Material.h"

#include "utils/Utils.h"

#include "math/math.h"

#include <fstream>
#include <iostream>
#include <optional>

namespace erm {
	
	bool ModelUtils::ParseModel(
		const char* path,
		std::deque<Model>& modelContainer,
		std::deque<Material>& materialContainer
	)
	{
		std::ifstream stream (Utils::GetRelativePath(path));
		
		if (!stream.is_open())
		{
			std::cout << "No such file: " << path << std::endl;
			return false;
		}
		
		std::string line;
		std::string name;
		std::string meshName;
		std::deque<Mesh> meshes;
		std::deque<Vertex> positions;
		std::deque<UVVertex> tPositions;
		std::deque<NormalVertex> nPositions;
		std::deque<VertexData> vertices;
		std::deque<IndexData> indices;
		Material* material;
		
		bool wasLooping = false;
		bool noMat = false;

		while (std::getline(stream, line))
		{
			std::vector<std::string> splitted = Utils::SplitString(line, ' ');

			if (splitted.size() > 0)
			{
				if (splitted[0].compare("o") == 0)
				{
					ASSERT(splitted.size() >= 2);
					name = splitted[splitted.size() - 1];
					
					if (!vertices.empty() && !indices.empty())
					{
						meshes.emplace_back(CreateMesh(vertices, indices, material, meshName));
						material = nullptr;
						vertices.clear();
						indices.clear();
						meshName.clear();
					}
				}
				else if (splitted[0].compare("g") == 0)
				{
					if (splitted.size() >= 2)
					{
						meshName = splitted[splitted.size() - 1];
					}
				}
				else if (splitted[0].compare("mtllib") == 0)
				{
					std::string pathStr (path);
					pathStr = pathStr.substr(0, pathStr.rfind("/"));
					pathStr.append("/");
					pathStr.append(splitted[1]);
					noMat = !ParseMaterialsLib(pathStr.c_str(), materialContainer);
				}
				else if (splitted[0].compare("v") == 0)
				{
					if (!vertices.empty() && !indices.empty() && material)
					{
						meshes.emplace_back(CreateMesh(vertices, indices, material, meshName));
						material = nullptr;
						vertices.clear();
						indices.clear();
					}
					
					if (wasLooping)
					{
						wasLooping = false;
						material = nullptr;
						vertices.clear();
						indices.clear();
					}
					
					ASSERT(splitted.size() >= 4);
					positions.emplace_back(
						std::atof(splitted[splitted.size() - 3].c_str()),
						std::atof(splitted[splitted.size() - 2].c_str()),
						std::atof(splitted[splitted.size() - 1].c_str())
					);
				}
				else if (splitted[0].compare("vt") == 0)
				{
					ASSERT(splitted.size() >= 3);
					tPositions.emplace_back(
						std::atof(splitted[splitted.size() - 2].c_str()),
						std::atof(splitted[splitted.size() - 1].c_str())
					);
				}
				else if (splitted[0].compare("vn") == 0)
				{
					ASSERT(splitted.size() >= 4);
					nPositions.emplace_back(
						std::atof(splitted[splitted.size() - 3].c_str()),
						std::atof(splitted[splitted.size() - 2].c_str()),
						std::atof(splitted[splitted.size() - 1].c_str())
					);
				}
				else if (splitted[0].compare("usemtl") == 0)
				{
					if (!vertices.empty() && !indices.empty())
					{
						meshes.emplace_back(CreateMesh(vertices, indices, material, meshName));
					}
					if (noMat)
					{
						material = &Material::DEFAULT;
						continue;
					}
					ASSERT(splitted.size() >= 2);
					std::string name = splitted[splitted.size() - 1];
					auto it = std::find_if(materialContainer.begin(), materialContainer.end(), [name](const Material& material) {
						return material.mName.compare(name) == 0;
					});
					material = it != materialContainer.end() ? &(*it) : nullptr;
				}
				else if (splitted[0].compare("f") == 0)
				{
					wasLooping = true;
					
					std::deque<VertexData> vVertices;
					ParseFace(vVertices, positions, tPositions, nPositions, splitted);
					
					std::deque<IndexData> vIndices;
					Triangulate(vIndices, vVertices);
					
					if (vIndices.empty())
					{
						continue;
					}
					
					for (unsigned int i = 0; i < vIndices.size(); ++i)
					{
						indices.emplace_back(vertices.size() + vIndices[i]);
					}
					
					for (unsigned int i = 0; i < vVertices.size(); ++i)
					{
						vertices.emplace_back(vVertices[i]);
					}
				}
			}
		}
		
		if (meshName.find("Collider") == std::string::npos && !vertices.empty() && !indices.empty())
		{
			meshes.emplace_back(CreateMesh(vertices, indices, material, meshName));
		}
		
		stream.close();
		
		if (name.empty())
		{
			name = "unknown";
		}
		
		modelContainer.emplace_back(
			path,
			name.c_str(),
			std::move(meshes)
		);
		
		return true;
	}
	
	void ModelUtils::ParseFace(
		std::deque<VertexData>& oVertices,
		const std::deque<Vertex>& positions,
		const std::deque<UVVertex>& tPositions,
		const std::deque<NormalVertex>& nPositions,
		const std::vector<std::string>& splitted
	)
	{
		static constexpr unsigned int kVertexIndex = 0;
		static constexpr unsigned int kUVVertexIndex = 1;
		static constexpr unsigned int kNormalVertexIndex = 2;
		
		for (const std::string& split: splitted)
		{
			if (split.empty() || split.compare("f") == 0)
			{
				continue;
			}
			
			const std::vector<std::string> indices = Utils::SplitString(split, '/');
			ASSERT(indices.size() <= 3);
			VertexData vertex;
			
			const unsigned int positionIndex = std::atoi(indices[kVertexIndex].c_str()) - 1;
			int tPositionIndex = -1;
			int nPositionIndex = -1;
			
			if (indices.size() == 2)
			{
				tPositionIndex = std::atoi(indices[kUVVertexIndex].c_str()) - 1;
			}
			else if (indices.size() == 3)
			{
				nPositionIndex = std::atoi(indices[kNormalVertexIndex].c_str()) - 1;
				
				if (!indices[kUVVertexIndex].empty())
				{
					tPositionIndex = std::atoi(indices[kUVVertexIndex].c_str()) - 1;
				}
			}
			
			ASSERT(positionIndex < positions.size());
			vertex.mVertex = positions[positionIndex];
			
			if (tPositionIndex >= 0 && tPositionIndex < tPositions.size())
			{
				ASSERT(tPositionIndex < tPositions.size());
				vertex.mUVVertex = tPositions[tPositionIndex];
			}
			else
			{
				vertex.mUVVertex = math::vec3(0.0f);
			}
			
			if (nPositionIndex >= 0 && nPositionIndex < nPositions.size())
			{
				ASSERT(nPositionIndex < nPositions.size());
				vertex.mNormalVertex = nPositions[nPositionIndex];
			}
			else
			{
				vertex.mNormalVertex = math::vec3(0.0f);
			}
			
			oVertices.emplace_back(vertex);
		}
	}
	
	void ModelUtils::Triangulate(
		std::deque<IndexData>& oIndices,
		const std::deque<VertexData>& vertices
	)
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
		
		const Vertex& a = vertices[0].mVertex;
		const Vertex& b = vertices[1].mVertex;
		const Vertex& c = vertices[2].mVertex;
		const Vertex& d = vertices[3].mVertex;
		
		if ((a == b && d == c) ||
			(a == d && b == c) ||
			(a == c && d == b))
		{
			return;
		}
		
		oIndices.push_back(0);
		oIndices.push_back(1);
		oIndices.push_back(3);
		
		Vertex d1 = c - a;
		Vertex d2 = d - b;
		Vertex intersection;
		
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
	
	Mesh ModelUtils::CreateMesh(
		const std::deque<VertexData>& vertices,
		const std::deque<IndexData>& indices,
		Material* material,
		const std::string& name
	)
	{
		Mesh mesh;
		
		mesh.mVerticesDataCount = vertices.size();
		mesh.mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mesh.mVerticesDataCount));
		for (unsigned int i = 0; i < mesh.mVerticesDataCount; ++i)
		{
			mesh.mVerticesData[i] = vertices[i];
		}
		
		mesh.mIndicesDataCount = indices.size();
		mesh.mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexData) * mesh.mIndicesDataCount));
		for (unsigned int i = 0; i < mesh.mIndicesDataCount; ++i)
		{
			mesh.mIndicesData[i] = indices[i];
		}
		
		mesh.mMaterial = material;
		mesh.mName = name;
		
		mesh.Setup();
		
		return mesh;
	}
	
	bool ModelUtils::ParseMaterialsLib(
		const char* path,
		std::deque<Material>& materialsContainer
	)
	{
		std::ifstream stream (Utils::GetRelativePath(path));
		
		if (!stream.is_open())
		{
			std::cout << "No such file: " << path << std::endl;
			return false;
		}
		
		std::string line;
		
		std::optional<Material> mat;
		mat.reset();
		bool skip = false;
		
		while (std::getline(stream, line))
		{
			std::vector<std::string> splitted = Utils::SplitString(line, ' ');
			
			if (splitted.size() > 0)
			{
				if (splitted[0].compare("newmtl") == 0)
				{
					if (!skip && mat)
					{
						materialsContainer.emplace_back(std::move(mat.value()));
					}
					
					mat.reset();
					skip = false;
					
					ASSERT(splitted.size() >= 2);
					std::string name = splitted[splitted.size() - 1];
					const auto& it = std::find_if(materialsContainer.begin(), materialsContainer.end(), [name](const Material& mat) {
						return mat.mName.compare(name) == 0;
					});
					if (it != materialsContainer.end())
					{
						skip = true;
						continue;
					}
					
					mat = Material();
					mat->mPath = path;
					mat->mName = name;
				}
				else if (skip)
				{
					continue;
				}
				else if (splitted[0].compare("Ka") == 0)
				{
					ASSERT(splitted.size() >= 4);
					mat->mAmbient = math::vec3(
						std::atof(splitted[splitted.size() - 3].c_str()),
						std::atof(splitted[splitted.size() - 2].c_str()),
						std::atof(splitted[splitted.size() - 1].c_str())
					);
				}
				else if (splitted[0].compare("Kd") == 0)
				{
					ASSERT(splitted.size() >= 4);
					mat->mDiffuse = math::vec3(
						std::atof(splitted[splitted.size() - 3].c_str()),
						std::atof(splitted[splitted.size() - 2].c_str()),
						std::atof(splitted[splitted.size() - 1].c_str())
					);
				}
				else if (splitted[0].compare("Ks") == 0)
				{
					ASSERT(splitted.size() >= 4);
					mat->mSpecular = math::vec3(
						std::atof(splitted[splitted.size() - 3].c_str()),
						std::atof(splitted[splitted.size() - 2].c_str()),
						std::atof(splitted[splitted.size() - 1].c_str())
					);
				}
				else if (splitted[0].compare("Ns") == 0)
				{
					ASSERT(splitted.size() >= 2);
					mat->mShininess = std::atof(splitted[splitted.size() - 1].c_str());
				}
				else if (splitted[0].compare("") == 0)
				{
					
				}
				else if (splitted[0].compare("") == 0)
				{
					
				}
				else if (splitted[0].compare("") == 0)
				{
					
				}
				else if (splitted[0].compare("") == 0)
				{
					
				}
			}
		}
		
		if (!skip && mat)
		{
			materialsContainer.emplace_back(std::move(mat.value()));
		}
		
		return true;
	}

}
