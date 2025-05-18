#pragma once

#include "erm/fs/FileLocator.h"

namespace erm {

class ERM_FileSystemImpl
{
public:
	bool init();
	bool deinit();

private:
	FileLocator mFileLocator;

};

} // namespace erm