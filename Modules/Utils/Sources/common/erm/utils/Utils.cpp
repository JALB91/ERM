#include "erm/utils/Utils.h"

#include <algorithm>
#include <cctype>
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
		ERM_ASSERT(cond);
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

	if (!ERM_EXPECT(stream.is_open(), "Failed to open file"))
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

	if (ERM_EXPECT(stream.is_open(), "Failed to open file"))
		stream.write(data.c_str(), data.size());
}

bool CompareNoCaseSensitive(const std::string& a, const std::string& b)
{
	if (a == b)
		return true;

	std::string buffA = a;
	std::transform(a.begin(), a.end(), buffA.begin(), [](unsigned char c) {
		return std::tolower(c);
	});

	std::string buffB = b;
	std::transform(b.begin(), b.end(), buffB.begin(), [](unsigned char c) {
		return std::tolower(c);
	});

	return buffA == buffB;
}

bool EndsWith(const std::string& s, const std::string& c)
{
	if (c.size() > s.size())
		return false;

	for (int i = static_cast<int>(c.size()); i > 0; --i)
		if (s[s.size() - i] != c[c.size() - i])
			return false;

	return true;
}

std::string FormatTime(unsigned int seconds)
{
	unsigned int minutes = seconds / 60;
	unsigned int hours = minutes / 60;
	minutes %= 60;
	seconds %= 60;

	std::string result;

	if (hours > 0)
		result += std::to_string(hours) + "h:";

	if (minutes > 0)
		result += std::to_string(minutes) + "m:";

	result += std::to_string(seconds) + "s";

	return result;
}

} // namespace erm::Utils
