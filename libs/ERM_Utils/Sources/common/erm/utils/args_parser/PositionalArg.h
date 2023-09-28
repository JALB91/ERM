#pragma once

#include "erm/utils/args_parser/ArgValue.h"
#include "erm/utils/args_parser/ArgValueType.h"

#include <erm/math/Types.h>

#include <optional>
#include <string>

namespace erm {

struct PositionalArg
{
	PositionalArg(
		u32 index,
		std::string_view name,
		ArgValueType valueType,
		std::optional<std::string>&& description = std::nullopt) noexcept;

	void print() const;

	u32 mIndex;
	str128 mName;
	ArgValueType mValueType;
	ArgValue mValue;
	std::optional<std::string> mDescription;
};

}