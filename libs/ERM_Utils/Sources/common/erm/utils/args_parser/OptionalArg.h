#pragma once

#include "erm/utils/args_parser/ArgValue.h"
#include "erm/utils/args_parser/ArgValueType.h"
#include "erm/utils/args_parser/IArgument.h"

namespace erm {

class OptionalArg : public IArgument
{
private:
	static constexpr str16 sValueText = str16(" <value>");
	
public:
	OptionalArg(
		std::optional<char> shortForm, 
		std::optional<std::string_view> namedForm, 
		ArgValue defaultValue, 
		std::string_view description = "No description provided");

	bool operator==(const OptionalArg& other) const;
	bool operator==(std::string_view str) const;
	bool operator==(char ch) const;

	inline const std::optional<char>& getShortForm() const
	{
		return mShortForm;
	}

	inline const std::optional<str32> getNamedForm() const
	{
		return mNamedForm;
	}
	
	u16 getTextLengthUntilDescription() const;
	
	void print(u16 maxNamedFormLength) const;

private:
	bool doesArgValueTypeExpectsInput() const;
	
	const std::optional<char> mShortForm;
	const std::optional<str32> mNamedForm;
	const std::string mDescription;
	
};

}
