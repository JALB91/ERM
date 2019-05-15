#include "utils/ModelUtils.h"

#include "rendering/Mesh.h"
#include "rendering/Model.h"
#include "rendering/IndexData.h"

#include "utils/Utils.h"

#include <fstream>
#include <iostream>

namespace erm {
	
	Model ModelUtils::ParseModel(const char* path)
	{
		std::ifstream stream (Utils::GetRelativePath(path));
		EXPECT(stream.is_open(), (std::string("No such file: ") + path).c_str());
		std::string line;
		std::string name;
		std::vector<Mesh> meshes;
		std::vector<Vertex> positions;
		std::vector<TextureVertex> tPositions;
		std::vector<VertexData> vertices;
		std::vector<IndexData> indices;
		
		bool looping = false;
		bool skip = false;

		while (std::getline(stream, line))
		{
			std::vector<std::string> splitted = Utils::SplitString(line, ' ');

			if (splitted.size() > 0)
			{
				if (std::strcmp(splitted[0].c_str(), "g") == 0 ||
					std::strcmp(splitted[0].c_str(), "o") == 0)
				{
					if (skip)
					{
						skip = false;
					}
					
					if (splitted[1].find("Collider") != std::string::npos ||
						splitted[1].find("collider") != std::string::npos)
					{
						skip = true;
						continue;
					}
					
					if (!looping)
					{
						looping = true;
						// TODO
					}
					else
					{
						if (!vertices.empty() && !indices.empty())
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
							
							mesh.Setup();
							meshes.emplace_back(std::move(mesh));
							
							vertices.clear();
							indices.clear();
						}
					}
				}
				else if (skip)
				{
					continue;
				}
				else if (std::strcmp(splitted[0].c_str(), "v") == 0)
				{
					positions.emplace_back(
						std::atof(splitted[1].c_str()),
						std::atof(splitted[2].c_str()),
						std::atof(splitted[3].c_str())
					);
				}
				else if (std::strcmp(splitted[0].c_str(), "vt") == 0)
				{
					tPositions.emplace_back(
						std::atof(splitted[1].c_str()),
						std::atof(splitted[2].c_str())
					);
				}
				else if (std::strcmp(splitted[0].c_str(), "vn") == 0)
				{
					// TODO
				}
				else if (std::strcmp(splitted[0].c_str(), "f") == 0)
				{
					std::vector<VertexData> vVertices;
					ParseFace(vVertices, positions, tPositions, splitted);
					
					for (unsigned int i = 0; i < vVertices.size(); ++i)
					{
						vertices.push_back(vVertices[i]);
					}
					
					std::vector<IndexData> vIndices;
					Triangulate(vIndices, vVertices);
					
					for (unsigned int i = 0; i < vIndices.size(); ++i)
					{
						IndexData index = (vertices.size() - vVertices.size()) + vIndices[i];
						indices.push_back(index);
					}
				}
			}
		}
		
		if (!skip && !vertices.empty() && !indices.empty())
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
			
			mesh.Setup();
			meshes.emplace_back(std::move(mesh));
			
			vertices.clear();
			indices.clear();
		}

		stream.close();
		
		return Model(std::move(meshes));
	}
	
	void ModelUtils::ParseFace(
		std::vector<VertexData>& oVertices,
		const std::vector<Vertex>& positions,
		const std::vector<TextureVertex>& tPositions,
		const std::vector<std::string>& splitted
	)
	{
		for (const std::string& split: splitted)
		{
			if (std::strcmp(split.c_str(), "f") == 0)
			{
				continue;
			}
			const std::vector<std::string> indices = Utils::SplitString(split, '/');
			VertexData vertex {
				positions[std::atoi(indices[0].c_str()) - 1],
				tPositions[std::atoi(indices[1].c_str()) - 1]
			};
			oVertices.push_back(vertex);
		}
	}
	
	void ModelUtils::Triangulate(
		std::vector<IndexData>& oIndices,
		const std::vector<VertexData>& vertices
	)
	{
		if (vertices.size() < 3)
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
		
		std::vector<VertexData> tVerts = vertices;
		
		while (true)
		{
			for (unsigned int i = 0; i < tVerts.size(); ++i)
			{
				VertexData pPrev;
				
				if (i == 0)
				{
					pPrev = tVerts[tVerts.size() - 1];
				}
				else
				{
					pPrev = tVerts[i - 1];
				}
				
				VertexData pCur = tVerts[i];
				VertexData pNext;
				
				if (i == tVerts.size() - 1)
				{
					pNext = tVerts[0];
				}
				else
				{
					pNext = tVerts[i + 1];
				}
				
				if (tVerts.size() == 3)
				{
					for (unsigned int j = 0; j < tVerts.size(); ++j)
					{
						if (vertices[j].mVertex == pCur.mVertex)
						{
							oIndices.push_back(j);
						}
						if (vertices[j].mVertex == pPrev.mVertex)
						{
							oIndices.push_back(j);
						}
						if (vertices[j].mVertex == pNext.mVertex)
						{
							oIndices.push_back(j);
						}
					}
					
					tVerts.clear();
					break;
				}
				
				if (tVerts.size() == 4)
				{
					for (unsigned int j = 0; j < vertices.size(); ++j)
					{
						if (vertices[j].mVertex == pCur.mVertex)
						{
							oIndices.push_back(j);
						}
						if (vertices[j].mVertex == pPrev.mVertex)
						{
							oIndices.push_back(j);
						}
						if (vertices[j].mVertex == pNext.mVertex)
						{
							oIndices.push_back(j);
						}
					}
					
					glm::vec3 tempVec;
					
					for (unsigned int j = 0; j < tVerts.size(); ++j)
					{
						if (tVerts[j].mVertex != pCur.mVertex &&
							tVerts[j].mVertex != pPrev.mVertex &&
							tVerts[j].mVertex != pNext.mVertex)
						{
							tempVec = tVerts[j].mVertex;
							break;
						}
					}
					
					for (unsigned int j = 0; j < vertices.size(); ++j)
					{
						if (vertices[j].mVertex == pPrev.mVertex)
						{
							oIndices.push_back(j);
						}
						if (vertices[j].mVertex == pNext.mVertex)
						{
							oIndices.push_back(j);
						}
						if (vertices[j].mVertex == tempVec)
						{
							oIndices.push_back(j);
						}
					}
					
					tVerts.clear();
					break;
				}
				
				const float angleBeetween = Utils::AngleBetweenV3(pPrev.mVertex - pCur.mVertex, pNext.mVertex - pCur.mVertex);
				const float angle = angleBeetween * (180.0f / M_PI);
				
				if (angle <= 0.0f && angle >= 180.0f)
				{
					continue;
				}
				
				bool inTri = false;
				
				for (unsigned int j = 0; j < vertices.size(); ++j)
				{
					if (Utils::IsInTriangle(vertices[j].mVertex, pPrev.mVertex, pCur.mVertex, pNext.mVertex) &&
						vertices[j].mVertex!= pPrev.mVertex &&
						vertices[j].mVertex != pCur.mVertex &&
						vertices[j].mVertex != pNext.mVertex)
					{
						inTri = true;
						break;
					}
				}
				
				if (inTri)
				{
					continue;
				}
				
				for (unsigned int j = 0; j < vertices.size(); ++j)
				{
					if (vertices[j].mVertex == pCur.mVertex)
					{
						oIndices.push_back(j);
					}
					if (vertices[j].mVertex == pPrev.mVertex)
					{
						oIndices.push_back(j);
					}
					if (vertices[j].mVertex == pNext.mVertex)
					{
						oIndices.push_back(j);
					}
				}
				
				for (unsigned int j = 0; j < tVerts.size(); ++j)
				{
					if (tVerts[j].mVertex == pCur.mVertex)
					{
						tVerts.erase(tVerts.begin() + j);
						break;
					}
				}
				
				i = -1;
			}
			
			if (oIndices.size() == 0)
			{
				break;
			}
			
			if (tVerts.size() == 0)
			{
				break;
			}
		}
	}

}
