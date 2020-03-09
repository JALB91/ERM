#include "erm/utils/Utils.h"

#include <iostream>
#include <sstream>
#include <math.h>

namespace erm {
	
	namespace Utils {

		std::vector<std::string> SplitString(const std::string& str, char ch)
		{
			std::vector<std::string> res;
			std::stringstream ss (str);
			std::string segment;
			
			while (std::getline(ss, segment, ch))
			{
				res.emplace_back(segment);
			}

			return res;
		}
		
		std::string StripFunctionName(const char* fn)
		{
			std::string result = fn;
			
			result = result.substr(result.find("erm::") + 5);
			result = result.substr(0, result.find("("));
			
			return result;
		}
		
	}
	
}
