#pragma once

#include "erm/math/mat.h"

#include <string>
#include <vector>

namespace tinyxml2 {
	class XMLElement;
}

namespace erm {

	extern tinyxml2::XMLElement* FindChildWithAttribute(
		tinyxml2::XMLElement& element,
		const char* childName,
		const char* attributeName,
		const char* attributeValue);

	extern void ParseMatrix(
		std::vector<std::string>& values,
		unsigned int offset,
		math::mat4& matrix);

} // namespace erm
