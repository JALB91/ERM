#pragma once

#include "erm/args_parser/ArgValueType.h"
#include "erm/args_parser/IArgument.h"

#include <optional>
#include <string>
#include <string_view>

namespace erm {

class PositionalArg : public IArgument
{
public:
	PositionalArg(
		std::string_view name,
		ArgValueType valueType,
		std::string_view description = "No description provided");
	
	inline const std::string& getName() const
	{
		return mName;
	}

	void print() const;

private:
	const std::string mName;
	const std::string mDescription;
};

}
