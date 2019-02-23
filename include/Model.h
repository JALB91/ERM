#pragma once

#include <vector>

namespace erm {
	
	class Mesh;
	
	class Model
	{
	public:
		Model();
		
	private:
		std::vector<Mesh> mMeshes;
		
	};
	
}
