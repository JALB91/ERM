#include "erm/utils/Utils.h"

namespace erm::utils {

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
