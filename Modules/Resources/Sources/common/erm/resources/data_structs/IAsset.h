#pragma once

#include <string>
#include <string_view>

namespace erm {

struct IAsset
{
	IAsset(
		std::string_view path,
		std::string_view name)
		: mPath(path)
		, mName(name)
	{}

	std::string mPath;
	std::string mName;
};

} // namespace erm
