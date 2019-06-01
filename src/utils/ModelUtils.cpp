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
		std::vector<UVVertex> tPositions;
		std::vector<NormalVertex> nPositions;
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
					nPositions.emplace_back(
						std::atof(splitted[1].c_str()),
						std::atof(splitted[2].c_str()),
						std::atof(splitted[3].c_str())
					);
				}
				else if (std::strcmp(splitted[0].c_str(), "f") == 0)
				{
					std::vector<VertexData> vVertices;
					ParseFace(vVertices, positions, tPositions, nPositions, splitted);
					
					std::vector<IndexData> vIndices;
					Triangulate(vIndices, vVertices);
					
					if (vIndices.empty())
					{
						continue;
					}
					
					for (unsigned int i = 0; i < vVertices.size(); ++i)
					{
						vertices.push_back(vVertices[i]);
					}
					
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
			if (std::strcmp(split.c_str(), "f") == 0)
			{
				continue;
			}
			
			const std::vector<std::string> indices = Utils::SplitString(split, '/');
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
			
			vertex.mVertex = positions[positionIndex];
			
			if (tPositionIndex >= 0 && tPositionIndex < tPositions.size())
			{
				vertex.mUVVertex = tPositions[tPositionIndex];
			}
			
			if (nPositionIndex >= 0 && nPositionIndex < nPositions.size())
			{
				vertex.mNormalVertex = nPositions[nPositionIndex];
			}
			
			oVertices.emplace_back(vertex);
		}
	}
	
	bool Intersection(
		const Vertex& a,
		const Vertex& b,
		const Vertex& d1,
		const Vertex& d2,
		Vertex& intersection
	)
	{
		glm::mat<2, 2, VertexType> m1;
		m1[0][0] = a.x - b.x;
		m1[0][1] = -d1.x;
		m1[1][0] = a.y - b.y;
		m1[1][1] = -d1.y;
		
		glm::mat<2, 2, VertexType> m2;
		m2[0][0] = d2.x;
		m2[0][1] = -d1.x;
		m2[1][0] = d2.y;
		m2[1][1] = -d1.y;
		
		VertexType det2 = glm::determinant(m2);
		
		if (det2 == 0.0f)
		{
			m2[0][0] = d2.y;
			m2[0][1] = -d1.y;
			m2[1][0] = d2.z;
			m2[1][1] = -d1.z;
			
			det2 = glm::determinant(m2);
			
			if (det2 == 0.0f)
			{
				m2[0][0] = d2.x;
				m2[0][1] = -d1.x;
				m2[1][0] = d2.z;
				m2[1][1] = -d1.z;
				
				det2 = glm::determinant(m2);
				
				if (det2 == 0.0f)
				{
					return false;
				}
				
				m1[0][0] = a.x - b.x;
				m1[0][1] = -d1.x;
				m1[1][0] = a.z - b.z;
				m1[1][1] = -d1.z;
			}
			else
			{
				m1[0][0] = a.y - b.y;
				m1[0][1] = -d1.y;
				m1[1][0] = a.z - b.z;
				m1[1][1] = -d1.z;
			}
		}
		
		const VertexType det1 = glm::determinant(m1);
		const VertexType t = det1/det2;
		
		intersection = b + t * d2;
		
		return true;
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
		
		if (Intersection(a, b, d1, d2, intersection))
		{
			oIndices.push_back(1);
			oIndices.push_back(2);
			oIndices.push_back(3);
			return;
		}
		
		d1 = c - b;
		d2 = d - a;
		
		if (Intersection(b, a, d1, d2, intersection))
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

}
