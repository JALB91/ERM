#pragma once

#include "erm/utils/args_parser/ArgValueType.h"
#include "erm/utils/args_parser/IArgument.h"

#include <string_view>

namespace erm {

class OptionalArg : public IArgument
{
private:
	static constexpr std::string_view sValueText = " <value>";
	
public:
	OptionalArg(
		std::optional<char> shortForm, 
		std::optional<std::string_view> namedForm,
		ArgValueType valueType,
		std::string_view defaultValue,
		std::string_view description = "No description provided");

	bool operator==(const OptionalArg& other) const;
	bool operator==(std::string_view str) const;
	bool operator==(char ch) const;

	inline const std::optional<char>& getShortForm() const
	{
		return mShortForm;
	}

	inline const std::optional<std::string> getNamedForm() const
	{
		return mNamedForm;
	}

	inline std::string getName() const
	{
		return mNamedForm.value_or(std::string {mShortForm.value()});
	}
	
	u16 getTextLengthUntilDescription() const;
	
	void print(u16 maxNamedFormLength) const;

private:
	bool doesArgValueTypeExpectsInput() const;
	
	const std::optional<char> mShortForm;
	const std::optional<std::string> mNamedForm;
	const std::string mDescription;
	
};

}
