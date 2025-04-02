#pragma once

#include <erm/math/Types.h>

#include <type_traits>

namespace erm {

template<typename T>
constexpr u32 allFlags()
{
	u32 result = 0;
	for (u32 i = 1; i < static_cast<u32>(T::COUNT); i *= 2)
	{
		result |= i;
	}
	return result;
}

template<typename BitType>
class Flags
{
public:
	using MaskType = typename std::underlying_type_t<BitType>;

	// constructors
	Flags()
		: mMask(0)
	{}

	Flags(BitType bit)
		: mMask(static_cast<MaskType>(bit))
	{}

	Flags(Flags<BitType> const& rhs) = default;

	explicit Flags(MaskType flags)
		: mMask(flags)
	{}

	// relational operators
	bool operator<(Flags<BitType> const& rhs) const
	{
		return mMask < rhs.mMask;
	}

	bool operator<=(Flags<BitType> const& rhs) const
	{
		return mMask <= rhs.mMask;
	}

	bool operator>(Flags<BitType> const& rhs) const
	{
		return mMask > rhs.mMask;
	}

	bool operator>=(Flags<BitType> const& rhs) const
	{
		return mMask >= rhs.mMask;
	}

	bool operator==(Flags<BitType> const& rhs) const
	{
		return mMask == rhs.mMask;
	}

	bool operator!=(Flags<BitType> const& rhs) const
	{
		return mMask != rhs.mMask;
	}

	bool operator<(u32 rhs) const
	{
		return mMask < rhs;
	}

	bool operator<=(u32 rhs) const
	{
		return mMask <= rhs;
	}

	bool operator>(u32 rhs) const
	{
		return mMask > rhs;
	}

	bool operator>=(u32 rhs) const
	{
		return mMask >= rhs;
	}

	bool operator==(u32 rhs) const
	{
		return mMask == rhs;
	}

	bool operator!=(u32 rhs) const
	{
		return mMask != rhs;
	}

	// logical operator
	bool operator!() const
	{
		return !mMask;
	}

	// bitwise operators
	Flags<BitType> operator&(Flags<BitType> const& rhs) const
	{
		return Flags<BitType>(mMask & rhs.mMask);
	}

	Flags<BitType> operator|(Flags<BitType> const& rhs) const
	{
		return Flags<BitType>(mMask | rhs.mMask);
	}

	Flags<BitType> operator^(Flags<BitType> const& rhs) const
	{
		return Flags<BitType>(mMask ^ rhs.mMask);
	}

	Flags<BitType> operator~() const
	{
		return Flags<BitType>(mMask ^ allFlags<BitType>());
	}

	// assignment operators
	Flags<BitType>& operator=(Flags<BitType> const& rhs) = default;

	Flags<BitType>& operator|=(Flags<BitType> const& rhs)
	{
		mMask |= rhs.mMask;
		return *this;
	}

	Flags<BitType>& operator&=(Flags<BitType> const& rhs)
	{
		mMask &= rhs.mMask;
		return *this;
	}

	Flags<BitType>& operator^=(Flags<BitType> const& rhs)
	{
		mMask ^= rhs.mMask;
		return *this;
	}

	// cast operators
	explicit operator bool() const
	{
		return !!mMask;
	}

	explicit operator MaskType() const
	{
		return mMask;
	}

private:
	MaskType mMask;
};

} // namespace erm
