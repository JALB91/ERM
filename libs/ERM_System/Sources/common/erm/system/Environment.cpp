#include "erm/system/Environment.h"

#include <cstdlib>

namespace erm {

Environment::Environment() = default;

std::string_view Environment::get(std::string_view key) const
{
	return std::getenv(key.data());
}

}
