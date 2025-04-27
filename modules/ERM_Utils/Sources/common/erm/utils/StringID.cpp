#include "erm/utils/StringID.h"

#include <xxhash.h>

namespace erm {

namespace internal {

#ifdef ERM_DEBUG_STRING_IDS
constexpr auto kInvalidStrIdStr = "<INVALID_STR_ID>";
#endif
constexpr StringID::HashT kInvalidStrIdValue = 0;

} // namespace internal

StringID const StringID::INVALID = {};

StringID::StringID(std::string_view str /* = ""*/)
	: mHash(str.empty() ? internal::kInvalidStrIdValue : XXH3_64bits(static_cast<const void*>(str.data()), str.size()))
#ifdef ERM_DEBUG_STRING_IDS
	, mDebugString(str.empty() ? internal::kInvalidStrIdStr : str)
#endif
{}

StringID StringID::operator=(std::string_view str)
{
	mHash = str.empty() ? internal::kInvalidStrIdValue : XXH3_64bits(static_cast<const void*>(str.data()), str.size());
#ifdef ERM_DEBUG_STRING_IDS
	mDebugString = str.empty() ? internal::kInvalidStrIdStr : str;
#endif
	
	return *this;
}

} // namespace erm

