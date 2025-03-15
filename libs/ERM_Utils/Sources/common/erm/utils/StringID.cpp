#include "erm/utils/StringID.h"

#include <xxhash.h>

namespace erm {

StringID StringID::INVALID = {};

StringID::StringID(std::string_view str /* = ""*/)
	: mHash(str.empty() ? 0 : XXH3_64bits(static_cast<const void*>(str.data()), str.size()))
#ifdef ERM_DEBUG
	, mDebugString(str.empty() ? "INVALID_STR_ID" : str)
#endif
{}

StringID StringID::operator=(std::string_view str)
{
	mHash = str.empty() ? 0 : XXH3_64bits(static_cast<const void*>(str.data()), str.size());
#ifdef ERM_DEBUG
	mDebugString = str.empty() ? "INVALID_STR_ID" : str;
#endif
	
	return *this;
}

}

