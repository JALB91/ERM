#pragma once

#ifndef NDEBUG
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
	
	inline size_t GetHash() const
	{
		return mHash;
	}
	
	inline bool IsValid() const
	{
		return mHash != INVALID.mHash;
	}
	
#ifndef NDEBUG
	inline const std::string& GetDebugString() const
	{
		return mDebugString;
	}
#endif
	
private:
	size_t mHash;
#ifndef NDEBUG
	std::string mDebugString;
#endif
	
};

}

namespace std {

template <>
struct hash<erm::StringID>
{
	size_t operator()(const erm::StringID& k) const
	{
		return k.GetHash();
	}
};

}
