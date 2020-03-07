#include "erm/loaders/collada/ColladaLoaderUtils.h"

#include <tinyxml2.h>

using namespace tinyxml2;

namespace erm {
	
	XMLElement* FindChildWithAttribute(
		XMLElement& element,
		const char* childName,
		const char* attributeName,
		const char* attributeValue
	)
	{
		XMLElement* child = element.FirstChildElement(childName);
		
		while (child)
		{
			if (child->Attribute(attributeName) && std::strcmp(child->Attribute(attributeName), attributeValue) == 0)
			{
				return child;
			}
			
			child = child->NextSiblingElement(childName);
		}
		
		return nullptr;
	}
	
	void ParseMatrix(
		std::vector<std::string>& values,
		unsigned int offset,
		math::mat4& matrix
	)
	{
		matrix[0][0] = static_cast<float>(std::atof(values[offset*16+0].c_str()));
		matrix[1][0] = static_cast<float>(std::atof(values[offset*16+1].c_str()));
		matrix[2][0] = static_cast<float>(std::atof(values[offset*16+2].c_str()));
		matrix[3][0] = static_cast<float>(std::atof(values[offset*16+3].c_str()));

		matrix[0][1] = static_cast<float>(std::atof(values[offset*16+4].c_str()));
		matrix[1][1] = static_cast<float>(std::atof(values[offset*16+5].c_str()));
		matrix[2][1] = static_cast<float>(std::atof(values[offset*16+6].c_str()));
		matrix[3][1] = static_cast<float>(std::atof(values[offset*16+7].c_str()));

		matrix[0][2] = static_cast<float>(std::atof(values[offset*16+8].c_str()));
		matrix[1][2] = static_cast<float>(std::atof(values[offset*16+9].c_str()));
		matrix[2][2] = static_cast<float>(std::atof(values[offset*16+10].c_str()));
		matrix[3][2] = static_cast<float>(std::atof(values[offset*16+11].c_str()));

		matrix[0][3] = static_cast<float>(std::atof(values[offset*16+12].c_str()));
		matrix[1][3] = static_cast<float>(std::atof(values[offset*16+13].c_str()));
		matrix[2][3] = static_cast<float>(std::atof(values[offset*16+14].c_str()));
		matrix[3][3] = static_cast<float>(std::atof(values[offset*16+15].c_str()));
	}
	
}
