#pragma once

#include "erm/args_parser/ArgValueType.h"

#include <erm/utils/assert/Assert.h>

#include <string>
#include <variant>

namespace erm {

using ArgValue = std::variant<std::string, int, float, bool>;

static inline ArgValueType getArgValueTypeForArgValue(const ArgValue& argValue)
{
	if (std::holds_alternative<std::string>(argValue))
	{
		return ArgValueType::STRING;
	}
	else if (std::holds_alternative<int>(argValue))
	{
		return ArgValueType::INTEGER;
	}
	else if (std::holds_alternative<float>(argValue))
	{
		return ArgValueType::FLOAT;
	}
	else if (std::holds_alternative<bool>(argValue))
	{
		return ArgValueType::BOOLEAN;
	}

	ERM_LOG_CRITICAL("Invalid default value for optional argument");
	return ArgValueType::STRING;
}

}
