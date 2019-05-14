#include "utils/ModelUtils.h"

#include "rendering/Mesh.h"
#include "rendering/Model.h"
#include "rendering/VertexData.h"
#include "rendering/IndexData.h"

#include "utils/Utils.h"

#include <vector>
#include <array>
#include <fstream>
#include <iostream>

namespace erm {
	
	Model ModelUtils::ParseModel(const char* path)
	{
		std::ifstream stream (Utils::GetRelativePath(path));
		EXPECT(stream.is_open(), (std::string("No such file: ") + path).c_str());
		std::string line;
		std::vector<Vertex> vertices;
		std::vector<TextureVertex> textureVertices;
		std::vector<std::array<glm::vec<3, int>, 3>> indices;

		while (std::getline(stream, line))
		{
			std::vector<std::string> splitted = Utils::SplitString(line, ' ');

			if (splitted.size() > 0)
			{
				if (std::strcmp(splitted[0].c_str(), "g") == 0)
				{
					// TODO
				}
				else if (std::strcmp(splitted[0].c_str(), "v") == 0)
				{
					vertices.emplace_back(
						std::atof(splitted[1].c_str()),
						std::atof(splitted[2].c_str()),
						std::atof(splitted[3].c_str())
					);
				}
				else if (std::strcmp(splitted[0].c_str(), "vt") == 0)
				{
					textureVertices.emplace_back(
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
					const std::vector<std::string>& splitIndices1 = Utils::SplitString(splitted[1], '/');
					const std::vector<std::string>& splitIndices2 = Utils::SplitString(splitted[3], '/');
					const std::vector<std::string>& splitIndices3 = Utils::SplitString(splitted[2], '/');
					indices.emplace_back(
						std::array<glm::vec<3, int>, 3>{
							glm::vec<3, int>(
								std::atoi(splitIndices1[0].c_str()),
								std::atoi(splitIndices1[1].c_str()),
								std::atoi(splitIndices1[2].c_str())
							),
							glm::vec<3, int>(
								std::atoi(splitIndices2[0].c_str()),
								std::atoi(splitIndices2[1].c_str()),
								std::atoi(splitIndices2[2].c_str())
							),
							glm::vec<3, int>(
								std::atoi(splitIndices3[0].c_str()),
								std::atoi(splitIndices3[1].c_str()),
								std::atoi(splitIndices3[2].c_str())
							)
					}
					);
				}
			}
		}

		stream.close();
		
		int index = 0;
		Mesh mesh;
		mesh.mVerticesDataCount = indices.size() * 3;
		mesh.mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mesh.mVerticesDataCount));
		
		for (const std::array<glm::vec<3, int>, 3>& indicesData : indices)
		{
			for (const glm::vec<3, int>& indexData : indicesData)
			{
				VertexData vertexData;
				vertexData.mVertex = vertices[indexData[0] - 1];
				vertexData.mTextureVertex = textureVertices[indexData[1] - 1];
				
				mesh.mVerticesData[index++] = vertexData;
			}
		}
		
		index = 0;
		mesh.mIndicesDataCount = indices.size() * 3;
		mesh.mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexData) * mesh.mIndicesDataCount));
		
		for (int i = 0; i < mesh.mIndicesDataCount; ++i)
		{
			mesh.mIndicesData[i] = i;
		}
		
		mesh.Setup();

		return Model({ mesh });
	}

}
