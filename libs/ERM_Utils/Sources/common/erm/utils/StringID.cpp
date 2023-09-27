#include "erm/utils/StringID.h"

#include <functional>

static std::hash<std::string_view> hasher{};

namespace erm {

StringID StringID::INVALID = {};

StringID::StringID(std::string_view str /* = ""*/)
	: mHash(str.empty() ? 0 : hasher(str))
#ifdef ERM_DEBUG
	, mDebugString(str.empty() ? "INVALID_STR_ID" : str)
#endif
{}

StringID StringID::operator=(std::string_view str)
{
	mHash = str.empty() ? 0 : hasher(str);
#ifdef ERM_DEBUG
	mDebugString = str.empty() ? "INVALID_STR_ID" : str;
#endif
	
	return *this;
}

}

