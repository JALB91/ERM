#pragma once

#include "IModelLoader.h"

#include <string>

namespace erm {
	
	class ERMModelLoader: public IModelLoader
	{
	public:
		Model LoadModel(const std::string& modelPath) const override;
		
	private:
		std::string ParseModel(const std::string& modelPath) const;
		
	};
	
}
