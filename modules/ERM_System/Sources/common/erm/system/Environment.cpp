#include "erm/system/Environment.h"

#include <erm/fs/fs.h>

#include <erm/utils/assert/Assert.h>

#include <cstdlib>

namespace erm {

Environment::Environment() = default;

const char* Environment::get(const char* key) const
{
	if (!ERM_EXPECT(key != nullptr)) 
	{
		return nullptr;
	}

	return std::getenv(key);
}

int Environment::set(const char* key, const char* value) const
{
	if (!ERM_EXPECT(key != nullptr && value != nullptr)) 
	{
		return 1;
	}

#if defined(ERM_HOST_WINDOWS)
	return _putenv_s(key, value);
#endif
}

}
