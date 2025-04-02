#include "erm/nn/EnumStatementHandler.h"

#include "erm/nn/ParseHelper.h"
#include "erm/nn/Program.h"

namespace erm::nn {

struct EnumValue
{
	std::string mName;
	std::optional<std::string> mValue;
};

struct EnumStatement : public IStatement
{
	std::string mName;
	std::optional<std::string> mType;
	std::vector<EnumValue> mEnumValues;
	
	void print() const override
	{
		ERM_LOG("Enum:\n\tName: %s\n\tValues:\n", mName.c_str());
		
		for (const auto& enumValue : mEnumValues)
		{
			ERM_LOG(
				"\t\t%s%s%s,",
				enumValue.mName.c_str(),
				enumValue.mValue.has_value() ? " = " : "",
				enumValue.mValue.value_or("").data());
		}
	}
};

}

REFL_AUTO(
	type(erm::nn::EnumValue),
	field(mName),
	field(mValue, AssignInitialized)
);

REFL_AUTO(
	type(
		erm::nn::EnumStatement, 
		bases<erm::nn::IStatement>,
		erm::nn::StatementType{"enum"}, 
		erm::nn::DataContainer{"mEnumValues"}),
	field(mName),
	field(mType, ColonInitialized),
	field(mEnumValues, CommaSeparated)
);

static_assert(refl::is_reflectable<erm::nn::EnumValue>(), "");

namespace erm::nn {

EnumStatementHandler::EnumStatementHandler(std::string_view name)
	: IStatementHandler(name)
{}

std::unique_ptr<IStatement> EnumStatementHandler::parse(const Program& program, size_t& tokenIndex)
{
	auto result = ParseHelper::tryParseStatement<EnumStatement>(program, tokenIndex);
	return result;
	
	/*auto& tokens = program.getTokens();
	const auto& token = tokens[tokenIndex];
	
	ERM_ASSERT_HARD(token.mType == TokenType::IDENTIFIER && token.mValue == mName);
	
	if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
	{
		return nullptr;
	}
	
	const auto& identifier = tokens[tokenIndex];
	if (!ERM_EXPECT(identifier.mType == TokenType::IDENTIFIER, "Invalid token after enum declaration, expected identifier"))
	{
		return nullptr;
	}
	
	EnumStatement enumStm;
	enumStm.mName = identifier.mValue;
	
	if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
	{
		return nullptr;
	}
	
	const auto& scopeBegin = tokens[tokenIndex];
	if (!ERM_EXPECT(
		scopeBegin.mType == TokenType::SYMBOL && scopeBegin.mValue == "{",
		"Invalid token after enum's identifier, expected \"{\""))
	{
		return nullptr;
	}
	
	const Token* nextToken = nullptr;
	do
	{
		if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
		{
			return nullptr;
		}
		
		const bool wasValid = nextToken != nullptr;
		const bool wasIdentifier = wasValid && nextToken->mType == TokenType::IDENTIFIER;
		const bool wasComma = wasValid && nextToken->mType == TokenType::SYMBOL && nextToken->mValue == ",";
		const bool wasEquality = wasValid && nextToken->mType == TokenType::SYMBOL && nextToken->mValue == "=";
		const bool wasNumber = wasValid && nextToken->mType == TokenType::NUMERIC_LITERAL;
		
		nextToken = &tokens[tokenIndex];
		
		const bool isIdentifier = nextToken->mType == TokenType::IDENTIFIER;
		const bool isComma = nextToken->mType == TokenType::SYMBOL && nextToken->mValue == ",";
		const bool isEquality = nextToken->mType == TokenType::SYMBOL && nextToken->mValue == "=";
		const bool isNumber = nextToken->mType == TokenType::NUMERIC_LITERAL;
		
		if (wasValid)
		{
			if (isIdentifier && !wasComma)
			{
				ERM_LOG_ERROR("");
				return nullptr;
			}
			else if (isComma && !wasIdentifier & !wasNumber)
			{
				ERM_LOG_ERROR("");
				return nullptr;
			}
			else if (isNumber && !wasEquality)
			{
				ERM_LOG_ERROR("");
				return nullptr;
			}
			else if (isEquality && !wasIdentifier)
			{
				ERM_LOG_ERROR("");
				return nullptr;
			}
		}
		else if (!isIdentifier)
		{
			ERM_LOG_ERROR("");
			return nullptr;
		}
		
		if (isIdentifier)
		{
			enumStm.mEnumValues.emplace_back(EnumValue{.mName = nextToken->mValue.data()});
		}
		else if (isNumber)
		{
			enumStm.mEnumValues.back().mValue = nextToken->mValue;
		}
	}
	while (nextToken->mValue != "}");
	
	return std::make_unique<EnumStatement>(std::move(enumStm));*/
}

}
