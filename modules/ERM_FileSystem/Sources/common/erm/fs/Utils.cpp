#include "erm/fs/Utils.h"

#include <erm/log/Assert.h>

#include <fstream>

namespace erm::fs {

void writeToFile(std::string_view path, std::string_view data)
{
	std::ofstream stream(path.data());

	if (ERM_EXPECT(stream.is_open(), "Failed to open file: %s", path.data()))
	{
		stream.write(data.data(), data.size());
	}
}

std::string readFromFile(std::string_view path)
{
	std::ifstream stream(path.data());

	if (!ERM_EXPECT(stream.is_open(), "Failed to open file: %s", path.data()))
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

}
