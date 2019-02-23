#include "Model.h"
#include "Renderer.h"
#include "VertexData.h"
#include "Mesh.h"

#include <iostream>
#include <fstream>

namespace erm {
	
	Model::Model(const std::string& path)
		: mTranslation(0.0f)
		, mRotation(0.0f)
	{
		std::ifstream stream (path);
		
		ASSERT(stream.is_open());
		
		std::string vertices;
		
		while (getline(stream, vertices))
		{
			std::string indices;
			if (getline(stream, indices))
			{
				mMeshes.emplace_back(vertices, indices);
			}
		}
	}
	
	Model::~Model()
	{}
	
}
