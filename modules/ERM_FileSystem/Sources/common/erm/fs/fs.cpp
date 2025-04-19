#include "erm/fs/fs.h"

#include <erm/log/Log.h>

#include <mutex>

namespace erm::fs {

const path& getERMRoot()
{
	static const auto sERMRootDir = path(ERM_ROOT_DIR);
	return sERMRootDir;
}

}

