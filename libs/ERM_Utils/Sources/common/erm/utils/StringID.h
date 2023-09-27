#pragma once

#include <erm/math/Types.h>

#ifdef ERM_DEBUG
#include <string>
#endif
#include <string_view>

namespace erm {

class StringID
{
public:
	static StringID INVALID;
	
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
	
	inline u64 getHash() const
	{
		return mHash;
	}
	
	inline bool isValid() const
	{
		return mHash != INVALID.mHash;
	}
	
#ifdef ERM_DEBUG
	inline const std::string& getDebugString() const
	{
		return mDebugString;
	}
#endif
	
private:
	u64 mHash;
#ifdef ERM_DEBUG
	std::string mDebugString;
#endif
	
};

}

namespace std {

template <>
struct hash<erm::StringID>
{
	erm::u64 operator()(const erm::StringID& k) const
	{
		return k.getHash();
	}
};

}
