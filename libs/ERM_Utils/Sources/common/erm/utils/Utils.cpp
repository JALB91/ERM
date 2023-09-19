#include "erm/utils/Utils.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>

namespace erm::utils {

bool logCall(bool cond, std::string_view msg, std::string_view function, std::string_view file, int line)
{
	if (!cond)
	{
		std::cout << "[Assert] (" << msg << ") " << function << " " << file << ":" << line << std::endl;
		ERM_ASSERT(cond);
	}
	return cond;
}

std::vector<std::string> splitString(std::string_view str, char ch)
{
	std::vector<std::string> res;
	std::string* curr = str.empty() ? nullptr : &res.emplace_back();

	for (char c : str)
	{
		if (c == ch)
		{
			curr = &res.emplace_back();
		}
		else
		{
			*curr += c;
		}
	}

	return res;
}

std::string readFromFile(std::string_view path)
{
	std::ifstream stream(path.data());

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

void writeToFile(std::string_view path, std::string_view data)
{
	std::ofstream stream(path.data());

	if (ERM_EXPECT(stream.is_open(), "Failed to open file"))
	{
		stream.write(data.data(), data.size());
	}
}

bool compareNoCaseSensitive(std::string_view a, std::string_view b)
{
	if (a.size() != b.size())
		return false;
	
	for (u64 i = 0; i < a.size(); ++i)
	{
		if (std::tolower(a[i]) != std::tolower(b[i]))
		{
			return false;
		}
	}
	
	return true;
}

bool endsWith(std::string_view s, std::string_view c)
{
	if (c.size() > s.size())
	{
		return false;
	}

	for (int i = static_cast<int>(c.size()); i > 0; --i)
	{
		if (s[s.size() - i] != c[c.size() - i])
		{
			return false;
		}
	}

	return true;
}

std::string formatTime(u64 seconds)
{
	u64 minutes = seconds / 60;
	u64 hours = minutes / 60;
	minutes %= 60;
	seconds %= 60;

	std::string result;

	if (hours > 0)
	{
		result += std::to_string(hours) + "h:";
	}

	if (minutes > 0)
	{
		result += std::to_string(minutes) + "m:";
	}

	result += std::to_string(seconds) + "s";

	return result;
}

} // namespace erm::utils
