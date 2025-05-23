#include "erm/system/Environment.h"

#include <erm/log/Assert.h>

#include <cstdlib>

namespace erm {

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

#ifdef ERM_HOST_WINDOWS
	return _putenv_s(key, value);
#elifdef ERM_HOST_OSX
	return setenv(key, value, 0);
#endif
}

}
