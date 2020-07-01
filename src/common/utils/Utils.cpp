#include "erm/utils/Utils.h"

#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>

namespace erm::Utils {

	bool LogCall(bool cond, const char* msg, const char* function, const char* file, int line)
	{
		if (!cond)
		{
			std::cout << "[Assert] (" << msg << ") " << function << " " << file << ":" << line << std::endl;
			ASSERT(cond);
		}
		return cond;
	}

	std::vector<std::string> SplitString(const std::string& str, char ch)
	{
		std::vector<std::string> res;
		std::stringstream ss(str);
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

	std::string ReadFromFile(const char* path)
	{
		std::ifstream stream(path);

		if (!EXPECT(stream.is_open(), "Failed to open file"))
			return "";

		std::string result;
		std::string line;

		while (getline(stream, line))
		{
			result += line + "\n";
		}

		return result;
	}

	void WriteToFile(const char* path, const std::string& data)
	{
		std::ofstream stream(path);

		if (EXPECT(stream.is_open(), "Failed to open file"))
			stream.write(data.c_str(), data.size());
	}

} // namespace erm::Utils
