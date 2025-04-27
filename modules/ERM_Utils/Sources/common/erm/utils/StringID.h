#pragma once

#include <erm/math/Types.h>

#ifdef ERM_DEBUG_STRING_IDS
#include <string>
#endif
#include <string_view>

namespace erm {

class StringID
{
public:
	using HashT = u64;
	static const StringID INVALID;
	
public:
	StringID(std::string_view str = "");
	StringID operator=(std::string_view str);
	
	inline bool operator==(const StringID other) const
	{
		return mHash == other.mHash;
	}
	
	inline bool operator!=(const StringID other) const
	{
		return mHash != other.mHash;
	}
	
	inline HashT getHash() const
	{
		return mHash;
	}
	
	inline bool isValid() const
	{
		return mHash != INVALID.mHash;
	}
	

	inline std::string_view getDebugString() const
	{
#ifdef ERM_DEBUG_STRING_IDS
		return mDebugString;
#endif
		return "<UNKNOWN_STR_ID>";
	}
	
private:
	HashT mHash;
#ifdef ERM_DEBUG_STRING_IDS
	std::string mDebugString;
#endif
	
};

}

namespace std {

template <>
struct hash<erm::StringID>
{
	erm::StringID::HashT operator()(const erm::StringID& k) const
	{
		return k.getHash();
	}
};

}
