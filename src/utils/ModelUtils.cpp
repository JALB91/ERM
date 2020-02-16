#include "utils/ModelUtils.h"

#include "rendering/Mesh.h"
#include "rendering/Model.h"
#include "rendering/Material.h"

#include "utils/Utils.h"

#include "math/math.h"

#include <deque>
#include <future>
#include <thread>
#include <fstream>
#include <iostream>
#include <optional>
#include <algorithm>

namespace {

	std::deque<erm::Model*> loadingModels;
	std::deque<std::future<void>> futures;
	std::atomic<bool> stop = false;
	std::mutex mut;
	std::mutex mut2;

}

namespace erm {

	void ModelUtils::OnUpdate()
	{
		if (!mut2.try_lock())
		{
			return;
		}
		
		for (erm::Model* model : loadingModels)
		{
			for (Mesh& mesh : model->GetMeshes())
			{
				if (!mesh.IsReady())
				{
					mesh.Setup();
				}
			}
		}
		
		mut2.unlock();

		for (int i = 0; i < static_cast<int>(futures.size()); ++i)
		{
			std::future<void>& future = futures[i];

			if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				futures.erase(futures.begin() + i);
				loadingModels.erase(loadingModels.begin() + i);
				--i;
			}
		}
	}
	
	void ModelUtils::OnRender()
	{
		mut.lock();
		mut2.lock();
	}
	
	void ModelUtils::OnPostRender()
	{
		mut.unlock();
		mut2.unlock();
	}

	void ModelUtils::OnDestroy()
	{
		stop = true;
		for (const std::future<void>& future : futures)
		{
			future.wait();
		}
		futures.clear();
		loadingModels.clear();
	}
	
	bool ModelUtils::ParseModel(
		const char* path,
		Models& models,
		Materials& materials
	)
	{
		std::ifstream stream (Utils::GetRelativePath(path));
		
		if (!stream.is_open())
		{
			std::cout << "No such file: " << path << std::endl;
			return false;
		}

		stream.close();

		mut.lock();
		mut2.lock();
		Model& model = *models.emplace_back(std::make_unique<Model>(path, "unknown"));
		mut.unlock();
		mut2.unlock();
		
		loadingModels.emplace_back(&model);
		futures.emplace_back(
			std::async(
				std::launch::async,
				&ModelUtils::ParseModelInt,
				path,
				std::ref(model),
				std::ref(materials)
			)
		);

		return true;
	}

	void ModelUtils::ParseModelInt(
		const char* path,
		Model& model,
		Materials& materials
	)
	{
		std::ifstream stream(Utils::GetRelativePath(path));

		std::string line;
		std::string name;
		std::string meshName;
		std::vector<Vertex> positions;
		std::vector<UVVertex> tPositions;
		std::vector<NormalVertex> nPositions;
		std::vector<VertexData> vertices;
		std::vector<IndexData> indices;
		Material* material;

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

					if (!vertices.empty() && !indices.empty())
					{
						AddMesh(model, vertices, indices, material, meshName);
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
					std::string pathStr(path);
					pathStr = pathStr.substr(0, pathStr.rfind("/"));
					pathStr.append("/");
					pathStr.append(splitted[1]);
					noMat = !ParseMaterialsLib(pathStr.c_str(), materials);
				}
				else if (splitted[0].compare("v") == 0)
				{
					if (!vertices.empty() && !indices.empty() && material)
					{
						AddMesh(model, vertices, indices, material, meshName);
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
						AddMesh(model, vertices, indices, material, meshName);
					}
					if (noMat)
					{
						material = &Material::DEFAULT;
						continue;
					}
					ASSERT(splitted.size() >= 2);
					std::string name = splitted[splitted.size() - 1];
					mut.lock();
					auto it = std::find_if(materials.begin(), materials.end(), [name](const std::unique_ptr<Material>& material) {
						return material->mName.compare(name) == 0;
					});
					material = it != materials.end() ? (*it).get() : nullptr;
					mut.unlock();
				}
				else if (splitted[0].compare("f") == 0)
				{
					wasLooping = true;

					std::vector<VertexData> vVertices;
					ParseFace(vVertices, positions, tPositions, nPositions, splitted);

					std::vector<IndexData> vIndices;
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

		stream.close();

		if (meshName.find("Collider") == std::string::npos && !vertices.empty() && !indices.empty())
		{
			AddMesh(model, vertices, indices, material, meshName);
		}

		if (name.empty())
		{
			name = "unknown";
		}

		model.SetName(name);
	}
	
	void ModelUtils::ParseFace(
		std::vector<VertexData>& oVertices,
		const std::vector<Vertex>& positions,
		const std::vector<UVVertex>& tPositions,
		const std::vector<NormalVertex>& nPositions,
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
			
			if (tPositionIndex >= 0 && tPositionIndex < static_cast<int>(tPositions.size()))
			{
				ASSERT(tPositionIndex < static_cast<int>(tPositions.size()));
				vertex.mUVVertex = tPositions[tPositionIndex];
			}
			else
			{
				vertex.mUVVertex = math::vec3(0.0f);
			}
			
			if (nPositionIndex >= 0 && nPositionIndex < static_cast<int>(nPositions.size()))
			{
				ASSERT(nPositionIndex < static_cast<int>(nPositions.size()));
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
		std::vector<IndexData>& oIndices,
		const std::vector<VertexData>& vertices
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
	
	void ModelUtils::AddMesh(
		Model& model,
		std::vector<VertexData>& vertices,
		std::vector<IndexData>& indices,
		Material* material,
		std::string& meshName
	)
	{
		mut.lock();
		mut2.lock();
		model.AddMesh(CreateMesh(vertices, indices, material, meshName));
		mut.unlock();
		mut2.unlock();
	}
	
	Mesh ModelUtils::CreateMesh(
		const std::vector<VertexData>& vertices,
		const std::vector<IndexData>& indices,
		Material* material,
		const std::string& name
	)
	{
		Mesh mesh (DrawMode::TRIANGLES);
		
		mesh.mVerticesDataCount = vertices.size();
		mesh.mVerticesData = new VertexData[mesh.mVerticesDataCount];
		for (unsigned int i = 0; i < mesh.mVerticesDataCount; ++i)
		{
			mesh.mVerticesData[i] = vertices[i];
		}
		
		mesh.mIndicesDataCount = indices.size();
		mesh.mIndicesData = new IndexData[mesh.mIndicesDataCount];
		for (unsigned int i = 0; i < mesh.mIndicesDataCount; ++i)
		{
			mesh.mIndicesData[i] = indices[i];
		}
		
		mesh.mMaterial = material;
		mesh.mName = name;
		
		return mesh;
	}
	
	bool ModelUtils::ParseMaterialsLib(
		const char* path,
		Materials& materials
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
		bool skip = false;
		
		while (std::getline(stream, line))
		{
			if (stop)
			{
				stream.close();
				return false;
			}

			const std::vector<std::string> splitted = Utils::SplitString(line, ' ');
			
			if (splitted.size() > 0)
			{
				if (splitted[0].compare("newmtl") == 0)
				{
					if (!skip && mat)
					{
						mut.lock();
						materials.emplace_back(std::make_unique<Material>(std::move(mat.value())));
						mut.unlock();
					}
					
					mat.reset();
					skip = false;
					
					ASSERT(splitted.size() >= 2);
					std::string name = splitted[splitted.size() - 1];
					mut.lock();
					const auto& it = std::find_if(materials.begin(), materials.end(), [name](const std::unique_ptr<Material>& mat) {
						return mat->mName.compare(name) == 0;
					});
					mut.unlock();
					if (it != materials.end())
					{
						skip = true;
						continue;
					}
					
					mat = Material::DEFAULT;
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
					mat->mShininess = static_cast<float>(std::atof(splitted[splitted.size() - 1].c_str()));
				}
			}
		}

		stream.close();
		
		if (!skip && mat)
		{
			mut.lock();
			materials.emplace_back(std::make_unique<Material>(std::move(mat.value())));
			mut.unlock();
		}
		
		return true;
	}

}
