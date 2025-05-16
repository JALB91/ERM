#include "erm/system/Utils.h"

#include <erm/utils/StaticString.h>

namespace erm::system {

bool hasCommand(const char* cmd)
{
	str128 str(cmd);
#if defined(ERM_HOST_WINDOWS)
	str += " > NUL 2>&1";
#else
	str += " > /dev/null 2>&1";
#endif
	const int result = std::system(str.data());
	return result == EXIT_SUCCESS;
}

}
