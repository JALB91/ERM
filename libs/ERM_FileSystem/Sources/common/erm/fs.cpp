#include "erm/fs/fs.h"

#include <erm/utils/log/Log.h>

#include <mutex>

namespace erm::fs {

const path& getERMRoot()
{
	static path sERMRoot;
	static std::once_flag flag;
	
	std::call_once(flag, []() {
		auto cwd = current_path().string();
		if (const auto pos = cwd.find("ERM"); pos != std::string::npos)
		{
			cwd = cwd.substr(0, pos + 3);
		}
		else
		{
			ERM_LOG_ERROR("ERM root folder not found");
		}
		sERMRoot = cwd;
	});
	
	return sERMRoot;
}

}

