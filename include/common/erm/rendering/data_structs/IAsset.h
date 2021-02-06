#pragma once

#include <string>

namespace erm {

	struct IAsset
	{
		IAsset(
			const char* path,
			const char* name)
			: mPath(path)
			, mName(name)
		{}

		std::string mPath;
		std::string mName;
	};

} // namespace erm