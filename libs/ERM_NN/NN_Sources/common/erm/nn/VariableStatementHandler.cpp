#include "erm/nn/VariableStatementHandler.h"

#include "erm/nn/Program.h"
#include "erm/nn/ParseHelper.h"

#include <refl.hpp>

namespace erm::nn {

struct VariableStatement : public IStatement
{
	std::string mType;
	std::string mName;
	std::string mValue;
	
	void print() const override
	{
		ERM_LOG("Variable\n\tType: %s\n\tName: %s\n\tValue: %s\n", mType.c_str(), mName.c_str(), mValue.c_str());
	}
};

}

REFL_AUTO(
type(
	erm::nn::VariableStatement,
	bases<erm::nn::IStatement>,
	 erm::nn::StatementType{""})
)

namespace erm::nn {

VariableStatementHandler::VariableStatementHandler(std::string_view name)
	: IStatementHandler(name)
{}

std::unique_ptr<IStatement> VariableStatementHandler::parse(const Program& program, size_t& tokenIndex)
{
	auto& tokens = program.getTokens();
	const auto& token = tokens[tokenIndex];
	
	ERM_ASSERT_HARD(token.mType == TokenType::IDENTIFIER && token.mValue == mName);
	
	VariableStatement variable;
	variable.mType = token.mValue;
	
	if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
	{
		return nullptr;
	}
	
	const auto& identifier = tokens[tokenIndex];
	if (!ERM_EXPECT(identifier.mType == TokenType::IDENTIFIER, "Invalid token after \"%s\" declaration, expected identifier", mName.data()))
	{
		return nullptr;
	}
	
	variable.mName = identifier.mValue;
	
	if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
	{
		return nullptr;
	}
	
	const auto& nextToken = tokens[tokenIndex];
	if (nextToken.mType == TokenType::SYMBOL && nextToken.mValue == "=")
	{
		if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
		{
			return nullptr;
		}
		
		const auto& value = tokens[tokenIndex];
		if (!ERM_EXPECT(
			value.mType == TokenType::IDENTIFIER ||
			value.mType == TokenType::NUMERIC_LITERAL ||
			value.mType == TokenType::STRING_LITERAL,
			""))
		{
			return nullptr;
		}
		
		variable.mValue = value.mValue;
		
		if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
		{
			return nullptr;
		}
	}
	
	const auto& sc = tokens[tokenIndex];
	if (!ERM_EXPECT(sc.mType == TokenType::SYMBOL && sc.mValue == ";", "Invalid token after variable declaration, expected ';'"))
	{
		return nullptr;
	}
	
	return std::make_unique<VariableStatement>(std::move(variable));
}

}
