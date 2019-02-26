#pragma once

#include <string>

namespace erm {
	
	class Model;
	
	class IModelLoader
	{
	public:
		virtual ~IModelLoader() = default;
		
		virtual Model LoadModel(const std::string& path) const = 0;
		
	};
	
}
