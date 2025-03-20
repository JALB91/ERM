#include "erm/nn/ClassStatementHandler.h"

#include "erm/nn/Program.h"
#include "erm/nn/ParseHelper.h"

#include <refl.hpp>

#include <memory>
#include <string>
#include <vector>

namespace erm::nn {

struct ClassStatement : public IStatement
{
	std::string mName;
	std::vector<std::unique_ptr<IStatement>> mPublicStatements;
	std::vector<std::unique_ptr<IStatement>> mProtectedStatements;
	std::vector<std::unique_ptr<IStatement>> mPrivateStatements;
	
	void print() const override
	{
		ERM_LOG("Class\n\tName: %s\n", mName.c_str());
		ERM_LOG_INDENT();
		
		if (!mPublicStatements.empty())
		{
			ERM_LOG("Public statements:\n");
			ERM_LOG_INDENT();
			for (const auto& statement : mPublicStatements)
			{
				statement->print();
			}
			ERM_LOG_UNINDENT();
		}
		
		if (!mProtectedStatements.empty())
		{
			ERM_LOG("Protected statements:\n");
			ERM_LOG_INDENT();
			for (const auto& statement : mProtectedStatements)
			{
				statement->print();
			}
			ERM_LOG_UNINDENT();
		}
		
		if (!mPrivateStatements.empty())
		{
			ERM_LOG("Private statements:\n");
			ERM_LOG_INDENT();
			for (const auto& statement : mPrivateStatements)
			{
				statement->print();
			}
			ERM_LOG_UNINDENT();
		}
		ERM_LOG_UNINDENT();
		ERM_LOG_LINE();
	}
	
};

}

REFL_AUTO(
	type(
		erm::nn::ClassStatement,
		bases<erm::nn::IStatement>,
		erm::nn::StatementType{"class"},
		erm::nn::DataContainer{"mPublicStatements", "mPrivateStatements"}),
	field(mName),
	field(mPublicStatements, SemiColonSeparated),
	field(mProtectedStatements, SemiColonSeparated),
	field(mPrivateStatements, SemiColonSeparated)
);

namespace erm::nn {

ClassStatementHandler::ClassStatementHandler(std::string_view name)
	: IStatementHandler(name)
{}

std::unique_ptr<IStatement> ClassStatementHandler::parse(const Program& program, size_t& tokenIndex)
{
	auto result = ParseHelper::tryParseStatement<ClassStatement>(program, tokenIndex);
	return result;
	
	/*const auto& tokens = program.getTokens();
	const auto& token = tokens[tokenIndex];
	
	ERM_ASSERT_HARD(token.mType == TokenType::IDENTIFIER && token.mValue == mName);
	
	ClassStatement classStm;
	
	if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
	{
		return nullptr;
	}
	
	const auto& identifier = tokens[tokenIndex];
	if (!ERM_EXPECT(identifier.mType == TokenType::IDENTIFIER, "Invalid token after class declaration, expected identifier"))
	{
		return nullptr;
	}
	
	classStm.mName = identifier.mValue;
	
	if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
	{
		return nullptr;
	}
	
	const auto& scopeBegin = tokens[tokenIndex];
	if (!ERM_EXPECT(
		scopeBegin.mType == TokenType::SYMBOL && scopeBegin.mValue == "{",
		"Invalid token after class's identifier, expected \"{\""))
	{
		return nullptr;
	}
	
	auto* currVisibility = &classStm.mPrivateStatements;
	
	const Token* nextToken = nullptr;
	do
	{
		if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
		{
			return nullptr;
		}
		
		nextToken = &tokens[tokenIndex];
		
		if (nextToken->mType == TokenType::IDENTIFIER && nextToken->mValue == "public")
		{
			currVisibility = &classStm.mPublicStatements;
		}
		else if (nextToken->mType == TokenType::IDENTIFIER && nextToken->mValue == "protected")
		{
			currVisibility = &classStm.mProtectedStatements;
		}
		else if (nextToken->mType == TokenType::IDENTIFIER && nextToken->mValue == "private")
		{
			currVisibility = &classStm.mPrivateStatements;
		}
		else if (nextToken->mType == TokenType::IDENTIFIER)
		{
			const auto& handlers = program.getStatementHandlers();
			const auto it = handlers.find(nextToken->mValue.data());
			ERM_ASSERT_HARD(it != handlers.end());
			auto& [_, handler] = *it;
			
			auto statement = handler->parse(program, tokenIndex);
			if (!statement)
			{
				return nullptr;
			}
			
			currVisibility->emplace_back(std::move(statement));
		}
		else if (nextToken->mType != TokenType::SYMBOL || nextToken->mValue != "}")
		{
			ERM_LOG_ERROR("Invalid token detected, expecting '}'");
			return nullptr;
		}
	}
	while (nextToken->mType != TokenType::SYMBOL && nextToken->mValue != "}");
	
	return std::make_unique<ClassStatement>(std::move(classStm));*/
}

}
