#pragma once

#include <erm/math/Types.h>

#include <optional>

namespace erm {

struct QueueFamilyIndices
{
	inline bool isComplete() const
	{
		return (mGraphicsFamily.has_value() && mPresentFamily.has_value());
	}

	std::optional<u32> mGraphicsFamily;
	std::optional<u32> mPresentFamily;
};

} // namespace erm
