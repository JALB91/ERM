#include "erm/utils/args_parser/PositionalArg.h"

#include "erm/utils/assert/Assert.h"
#include "erm/utils/log/Log.h"

#include <iostream>

namespace erm {

PositionalArg::PositionalArg(
	u32 index,
	std::string_view name,
	ArgValueType valueType,
	std::optional<std::string>&& description /* = std::nullopt */) noexcept
	: mIndex(index)
	, mName(name)
	, mValueType(valueType)
	, mDescription(std::move(description))
{
	ERM_ASSERT_HARD(!mName.empty());
}

void PositionalArg::print() const
{
	ERM_LOG("<%s>: %s", mName, mDescription.value_or("No description provided").data());
}

}