#include "ERMModelLoader.h"
#include "Renderer.h"
#include "Utils.h"
#include "Mesh.h"

#include <iostream>
#include <fstream>

namespace erm {
	
	Model ERMModelLoader::LoadModel(const std::string& modelPath) const
	{
		std::vector<Mesh> meshes;
		std::string modelSrc = ParseModel(modelPath);
		std::vector<std::string> lines = SplitString(modelSrc, '\n');
		
		if (lines.size() > 0) {
			meshes.reserve(lines.size() / 4);
		}
		
		for (int i = 0; i < lines.size(); ++i)
		{
			std::string verticesSrc;
			std::string indicesSrc;
			
			ASSERT(lines[i].find("Vert") != std::string::npos);
			ASSERT(lines[i + 2].find("Ind") != std::string::npos);
			
			verticesSrc = lines[i + 1];
			indicesSrc = lines[i + 3];
			
			i += 3;
			
			meshes.emplace_back(verticesSrc, indicesSrc);
		}
		
		return Model(std::move(meshes));
	}
	
	std::string ERMModelLoader::ParseModel(const std::string &modelPath) const
	{
		std::ifstream stream (modelPath);
		
		ASSERT(stream.is_open());
		
		std::string result;
		std::string line;
		
		while (getline(stream, line))
		{
			result += line + "\n";
		}
		
		return result.substr(0, result.size() - 1);
	}
	
}
