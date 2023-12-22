#pragma once

#include "erm/utils/args_parser/ArgValue.h"
#include "erm/utils/args_parser/ArgValueType.h"

#include <magic_enum.hpp>

#include <optional>
#include <vector>

namespace erm {

class IArgument
{
public:
	IArgument(ArgValue defaultValue);
	virtual ~IArgument() = default;
	
	inline ArgValueType getValueType() const
	{
		return mValueType;
	}
	
	inline const ArgValue& getDefaultValue() const
	{
		return mDefaultValue;
	}
	
	inline const std::optional<ArgValue>& getValue() const
	{
		return mValue;
	}
	
	template<typename T, typename Enable = std::enable_if_t<!std::is_enum_v<T>>>
	const T& get() const
	{
		if constexpr (std::is_same_v<T, str64>)
		{
			ERM_ASSERT_HARD(mValueType == ArgValueType::STRING, "Arg value is not a string");
		}
		else if constexpr (std::is_same_v<T, int>)
		{
			ERM_ASSERT_HARD(mValueType == ArgValueType::INTEGER, "Arg value is not an integer");
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			ERM_ASSERT_HARD(mValueType == ArgValueType::FLOAT, "Arg value is not a float");
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			ERM_ASSERT_HARD(mValueType == ArgValueType::BOOLEAN, "Arg value is not a bool");
		}
		else
		{
			ERM_LOG_CRITICAL("Invalid type");
		}
		
		return mValue.has_value() ? std::get<T>(mValue.value()) : std::get<T>(mDefaultValue);
	}
	
	template<typename T, typename Enable = std::enable_if_t<std::is_enum_v<T>>>
	std::optional<T> get() const
	{
		ERM_ASSERT_HARD(mValueType == ArgValueType::STRING, "Only string arguments can be converted to enums");
		const auto& value = mValue.value_or(mDefaultValue);
		return magic_enum::enum_cast<T>(std::get<str64>(value).toUpper().data());
	}
	
	template<typename T, typename Enable = std::enable_if_t<std::is_enum_v<T>>>
	void setOptions()
	{
		mOptions = std::vector<str32>(magic_enum::enum_names<T>().begin(), magic_enum::enum_names<T>().end());
	}
	
	void setValue(ArgValue value);
	
protected:
	const ArgValueType mValueType;
	const ArgValue mDefaultValue;
	std::optional<ArgValue> mValue;
	std::vector<str32> mOptions;
	
};

}

