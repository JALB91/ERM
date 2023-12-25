#pragma once

#include "erm/utils/args_parser/ArgValueType.h"
#include "erm/utils/args_parser/IArgument.h"

#include <erm/math/Types.h>

#include <optional>
#include <string>

namespace erm {

class PositionalArg : public IArgument
{
public:
	PositionalArg(
		std::string_view name,
		ArgValueType valueType,
		std::string_view description = "No description provided");
	
	inline const str32& getName() const
	{
		return mName;
	}

	void print() const;

private:
	const str32 mName;
	const std::string mDescription;
};

}
