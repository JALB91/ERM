#include "Utils.h"

namespace erm {
	
	std::vector<std::string> SplitString(const std::string& str, char ch)
	{
		std::vector<std::string> vec;
		std::string res = "";
		
		for (auto iter = str.cbegin(); iter != str.cend(); ++iter)
		{
			if (*iter != ch)
			{
				res += *iter;
			}
			else
			{
				vec.push_back(res);
				res = "";
			}
		}
		
		vec.push_back(res);
		
		return vec;
	}
	
}
