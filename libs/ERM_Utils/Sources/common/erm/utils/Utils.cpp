#include "erm/utils/Utils.h"

#include "erm/utils/assert/Assert.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>

namespace erm::utils {

void writeToFile(std::string_view path, std::string_view data)
{
	std::ofstream stream(path.data());

	if (ERM_EXPECT(stream.is_open(), "Failed to open file"))
	{
		stream.write(data.data(), data.size());
	}
}

std::string readFromFile(std::string_view path)
{
	std::ifstream stream(path.data());

	if (!ERM_EXPECT(stream.is_open(), "Failed to open file"))
	{
		return "";
	}

	std::string result;
	std::string line;

	while (getline(stream, line))
	{
		result += line + "\n";
	}

	return result;
}

std::string formatTime(u64 seconds) noexcept
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

bool hasCommand(const char* const cmd)
{
	str128 str(cmd);
#if defined(ERM_WINDOWS)
	str += " > NUL 2>&1";
#else
	str += " > /dev/null 2>&1";
#endif
	const int result = std::system(str.data());
	return result == EXIT_SUCCESS;
}

static_assert(charToUpper('a') == 'A');
static_assert(charToUpper('B') == 'B');
static_assert(charToLower('C') == 'c');
static_assert(charToLower('c') == 'c');
static_assert(endsWith("CIAO", "AO"));
static_assert(compareNoCaseSensitive("CIAO", "ciao"));
static_assert(splitString("CIAO\nCIAO", '\n').size() == 2);
static_assert(splitString("CIAO\nCIAO\n\n", '\n').size() == 3);
static_assert(splitString("CIAO\nCIAO\n\n", '\n')[2].empty());
static_assert(splitString("\n\nCIAO\nCIAO\n\n", '\n')[4].empty());
static_assert(splitString("\nArguments:\n", '\n')[1] == "Arguments:");

} // namespace erm::utils
