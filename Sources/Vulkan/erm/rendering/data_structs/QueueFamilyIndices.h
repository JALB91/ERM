#pragma once

#include <optional>

namespace erm {

	struct QueueFamilyIndices
	{
		inline bool IsComplete() const
		{
			return (mGraphicsFamily.has_value() && mPresentFamily.has_value());
		}

		std::optional<uint32_t> mGraphicsFamily;
		std::optional<uint32_t> mPresentFamily;
	};

} // namespace erm
