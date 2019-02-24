#pragma once

#include "Model.h"

#include <string>

namespace erm {
	
	class IModelLoader
	{
	public:
		virtual ~IModelLoader() = default;
		
		virtual Model LoadModel(const std::string& path) const = 0;
		
	};
	
}
