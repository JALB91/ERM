#pragma once

#include "erm/utils/args_parser/ArgValue.h"
#include "erm/utils/args_parser/ArgValueType.h"

#include <optional>

namespace erm {

class OptionalArg
{
public:
	OptionalArg(ArgValueType valueType, char shortForm, str64 namedForm, ArgValue defaultValue);
	OptionalArg(ArgValueType valueType, str64 namedForm, ArgValue defaultValue);
	OptionalArg(ArgValueType valueType, char shortForm, ArgValue defaultValue);

	bool operator==(const OptionalArg& other) const;
	bool operator==(std::string_view str) const;
	bool operator==(char ch) const;

	inline ArgValueType getValueType() const
	{
		return mValueType;
	}

	inline const std::optional<ArgValue>& getValue() const
	{
		return mValue;
	}

	inline const ArgValue& getDefaultValue() const
	{
		return mDefaultValue;
	}

	inline const std::optional<char>& getShortForm() const
	{
		return mShortForm;
	}

	inline const std::optional<str64> getNamedForm() const
	{
		return mNamedForm;
	}
	
	void setValue(ArgValue&& value);
	void setDescription(std::string_view description);

	void print(u16 maxNamedFormLength) const;

private:
	const ArgValueType mValueType;

	const std::optional<char> mShortForm;
	const std::optional<str64> mNamedForm;

	std::optional<ArgValue> mValue;
	const ArgValue mDefaultValue;

	std::string mDescription;
};

}