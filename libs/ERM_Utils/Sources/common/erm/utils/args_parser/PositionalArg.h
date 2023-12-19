#pragma once

#include "erm/utils/args_parser/ArgValue.h"
#include "erm/utils/args_parser/ArgValueType.h"

#include <erm/math/Types.h>

#include <optional>
#include <string>

namespace erm {

class PositionalArg
{
public:
	PositionalArg(
		std::string_view name,
		ArgValueType valueType,
		std::string_view description = "No description provided");

	void print() const;

	inline ArgValueType getValueType() const
	{
		return mValueType;
	}

	inline const ArgValue& getValue() const
	{
		return mValue;
	}

	inline const str32& getName() const
	{
		return mName;
	}

	void setValue(ArgValue&& value);

private:
	const str32 mName;
	const ArgValueType mValueType;
	const std::string mDescription;
	ArgValue mValue;
};

}