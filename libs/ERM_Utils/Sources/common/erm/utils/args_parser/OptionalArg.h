#pragma once

#include "erm/utils/args_parser/ArgValue.h"
#include "erm/utils/args_parser/ArgValueType.h"

namespace erm {

struct OptionalArg
{
	OptionalArg(ArgValueType valueType, char shortForm) noexcept;
	OptionalArg(ArgValueType valueType, str128 namedForm) noexcept;
	OptionalArg(ArgValueType valueType, char shortForm, str128 namedForm) noexcept;

	bool operator==(const OptionalArg& other) const noexcept;

	void print(u16 maxNamedFormLength) const;

	const ArgValueType mValueType;

	const std::optional<char> mShortForm;
	const std::optional<str128> mNamedForm;

	ArgValue mValue;
	ArgValue mDefaultValue;

	std::optional<std::string> mDescription;
};

}